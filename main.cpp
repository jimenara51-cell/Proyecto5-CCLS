#include <graphics.h>
#include <conio.h>
#include <dos.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

union REGS mouseData;

// Obtiene la posición actual del mouse y determina si el botón izquierdo está presionado
void obtenerMouse(int *x, int *y, int *clic) {
    mouseData.x.ax = 3;
    int86(0x33, &mouseData, &mouseData);

    *x = mouseData.x.cx;
    *y = mouseData.x.dx;

    *clic = mouseData.x.bx & 1;
}

// Inicializa el mouse
void iniciarMouse() {
    mouseData.x.ax = 0;
    int86(0x33, &mouseData, &mouseData);
}

// Muestra el cursor del mouse
void mostrarMouse() {
    mouseData.x.ax = 1;
    int86(0x33, &mouseData, &mouseData);
}

// Oculta el cursor del mouse
void ocultarMouse() {
    mouseData.x.ax = 2;
    int86(0x33, &mouseData, &mouseData);
}

// Genera una nueva posición aleatoria para el objetivo
void generarObjetivo(int *x, int *y, int tamano) {
    // Hay que evitar la parte superior de la pantalla, donde se muestran el título, el contador y el tiempo
    int ancho = getmaxx();
    int alto = getmaxy();
    
    *x = random(ancho - 30) + 10; // 10 hasta 640
    *y = random(alto - 110) + 80; // 80 hasta 480
}

// Dibuja el objetivo
void dibujarObjetivo(int x, int y, int tamano) {
    setfillstyle(SOLID_FILL, RED); //Color
    bar(x, y, x + tamano, y + tamano);// Un cuadrado
}

// Dibuja el título, contador y tiempo
void mostrarInformacion(int objetivo, int total, float tiempo) {
    char texto[80];

    setcolor(WHITE);

    settextstyle(DEFAULT_FONT, HORIZ_DIR, 1);
    outtextxy(10, 10, "MINIJUEGO");

    sprintf(texto, "Objetivos: %d/%d", objetivo, total);
    outtextxy(10, 35, texto);

    sprintf(texto, "Tiempo: %.2f s", tiempo);
    outtextxy(520, 35, texto);
}

int main() {
    int driver = DETECT;
    int modo;

    int xMouse;
    int yMouse;
    int clic;
    int clicAnterior = 0;

    int tiempoActivo = 0;
    clock_t inicio;
    clock_t tiempoActual;
    clock_t tiempoFinal;

    float segundos;

    // Inicializar gráficos
    initgraph(&driver, &modo, "C://TC//BGI");

    // Verificar que los gráficos se hayan iniciado correctamente
    if (graphresult() != grOk) {
        printf("No se pudo iniciar el modo grafico.");
        getch();
        return 1;
    }

    // Inicializar el generador de números aleatorios
    randomize();
    
    // Inicializar y mostrar el mouse
    iniciarMouse();
    mostrarMouse();

    // Primera posición del objetivo
    int xObjetivo;
    int yObjetivo;
    int tamanoObjetivo = 15;
    generarObjetivo(&xObjetivo, &yObjetivo, tamanoObjetivo);

    // El juego termina cuando se completan 10 objetivos
    int objetivo = 0;
    while (objetivo < 10) {
        // Limpiar la pantalla
        cleardevice();

        // Calcular el tiempo transcurrido. El cronómetro solo funciona después del primer objetivo acertado
        segundos = 0;
        if (tiempoActivo == 1){
            tiempoActual = clock();
            segundos = (float)(tiempoActual - inicio) / CLOCKS_PER_SEC;
        }

        // Mostrar información
        mostrarInformacion(objetivo, 10, segundos);

        // Dibujar objetivo
        dibujarObjetivo(xObjetivo, yObjetivo, tamanoObjetivo);

        // Obtener posición y estado del mouse
        obtenerMouse(&xMouse, &yMouse, &clic);
        
        
        /*
            Detectar un nuevo clic
            Se verifica que:
            - el botón esté presionado ahora
            - antes no estuviera presionado
        */
        if (clic == 1 && clicAnterior == 0) {
            // Verificar si el clic fue dentro del objetivo
            if (xMouse >= xObjetivo && xMouse <= xObjetivo + tamanoObjetivo && yMouse >= yObjetivo && yMouse <= yObjetivo + tamanoObjetivo) {
                // Si es el primer objetivo acertado, comienza el cronómetro
                if (tiempoActivo == 0){
                    inicio = clock();
                    tiempoActivo = 1;
                }

                // Aumentar la cantidad de objetivos completados
                objetivo++;

                // Generar una nueva posición
                if (objetivo < 10);
                generarObjetivo(&xObjetivo, &yObjetivo, tamanoObjetivo);    
            


            }
        }

        // Guardar el estado anterior del clic
        clicAnterior = clic;

        // Pequeña pausa para evitar que el programa consuma todos los recursos del equipo
        delay(20);
    }

    // Ocultar mouse mientras se muestra la pantalla final
    ocultarMouse();
    
    cleardevice();

    settextstyle(DEFAULT_FONT, HORIZ_DIR, 1);
    outtextxy(240, 150, "JUEGO TERMINADO");

    char textoFinal[50];
    sprintf(textoFinal, "Tiempo: %.2f segundos", segundos);
    outtextxy(220, 240, textoFinal);

    outtextxy(205, 290, "Presione una tecla para salir");

    // Esperar antes de cerrar
    getch();

    closegraph();

    return 0;
}
