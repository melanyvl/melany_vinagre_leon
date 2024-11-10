/****************************************************************\
   ================================================================
                    JUEGO DEL DINOSAURIO 


  Hecho por:Melany Vinagre León 
  ================================================================
  \****************************************************************/

  //--------------.--------IMPORTAMOS BIBLIOTECAS NECESARIAS PARA EL JUEGO --------------------------------
  #include <LiquidCrystal.h>// para manejar una pantalla LCD. (La pantalla LCD es donde veremos el juego. )
  #include <Keypad.h>// nos enseña cómo detectar cuando presionamos un botón (en este juego no se usa mucho)
  #include <EEPROM.h>//nos ayuda a guardar la puntuación más alta en un lugar seguro (memoria externa), incluso si apagamos el Arduino.

 
  // Comenta esta sección si estás ejecutando el código con un solo botón(en mi caso yo siempre uso dos  botones de la pantalla : up y reset ).
  
  #define CMK_HARDWARE
  
  const int eepromAddress = 0; // direccion de la eeprom para guaradar el valor del record sin que se pierda
  int valorrecord = 0; // valor inicial que vamos a meter en la eeprom para luego modificarlo(todas las veces que se actualice )
  
  
  #ifdef CMK_HARDWARE
    // LCD pins
    #define RS 8    // LCD reset pin
    #define En 9    // LCD enable pin
    #define D4 4    // LCD data pin 4
    #define D5 5    // LCD data pin 5
    #define D6 6    // LCD data pin 6
    #define D7 7    // LCD data pin 7    
  #else
    // LCD pins
    #define RS 12   // LCD reset pin
    #define En 11   // LCD enable pin
    #define D4 5    // LCD data pin 4
    #define D5 4    // LCD data pin 5
    #define D6 3    // LCD data pin 6
    #define D7 2    // LCD data pin 7
  #endif

  //inicializamos la pantalla LCD
  LiquidCrystal lcd(RS, En, D4, D5, D6, D7);

  // puntos 
  int cuadrados = 0; // numero de cuadraditos totales que consigues pasar en una partida en el juego( en la pantalla lo tenemos puesto con el nombre "score")


  // Limitamos la frecuencia de salto( porque si no ponemos un limite el personaje podria mantenerse arriba todo el tiempo que quisieses y no tendria complicacion el juego)
  bool salto = false;



//-------PERSONAJES NECESARIAS EN EL JUEGO----------
// ----- Vamos a configurar cada dibujo que aparece en el juego para que pueda aparecer bien en la pantalla LCD 

  // dinosaurio con pierna derecha 
  byte dino_r[8] = {
    B00000111,
    B00000101,
    B00000111,
    B00010110,
    B00011111,
    B00011110,
    B00001110,
    B00000010
  };

  // dinosaurio con pierna izquierda
  byte dino_l[8] = {
    B00000111,
    B00000101,
    B00000111,
    B00010110,
    B00011111,
    B00011110,
    B00001110,
    B00000100
  };
   //cactus grande 
  byte cactus_big[8] = {
    B00000000,
    B00000100,
    B00000101,
    B00010101,
    B00010110,
    B00001100,
    B00000100,
    B00000100
  };

  // Cactus pequeño 
  byte cactus_small[8] = {
    B00000000,
    B00000000,
    B00000100,
    B00000101,
    B00010101,
    B00010110,
    B00001100,
    B00000100
  };

 

  // configuramos el mundo para crear una pequeña parte el  juego.(CHAR WORLD ) 
  //Cada número dentro del arreglo representa un elemento o carácter que se mostrará en la pantalla
  //  112, 117, 110, 116, 111, 115 es la palabra puntos  que utilizaos para imprimir por pantalla la variable cuadrados
  // el numero 32 es espacio en blanco 
  //el numero 0 es un dinosaurio y el 3 es un cactus 
  char world[] = {
    32, 32, 32, 32, 32, 32, 32, 112, 117, 110, 116, 111, 115, 32, 32, 32,
    32, 0, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
};


  // bucle de juego infinito 
  uint8_t scroll_world() {
    // en el hardware real necesitamos un retraso (delay 250)
    #ifdef CMK_HARDWARE// comprobamos si esta definida la macro ( para un uso determinado)
      delay(250);// rl ptograma se pausa durante 0.25 segundos
    #endif



    //Esta línea de código decide si el siguiente objeto que aparecerá en la pantalla será un cactus pequeño, un cactus grande o un espacio vacío.
    //-----------POSIBLES CASOS Y QUE AARECERA EN PANTALLA -------------
    // Si el número es muy pequeño (cerca del 2), aparece un cactus grande.
    //Si el número es un poco más grande, aparece un cactus pequeño.
    //Si el número es muy grande (cerca del 34), aparece un espacio vacío.
    // el numero que sale se almacena en la variable que esta definida como objeto_aleatorio
    char objeto_aleatorio = random(2, 35);  


    // lugar del cactus en el mapa 
    if (objeto_aleatorio< 4) world[31] = objeto_aleatorio;//si aparece esto se pondra un cactus en la posicion 31 ( aparecera en la pantalla )

    // si no lo que nos indica este codigon es que en esa posicion (si el numero aleatorio es >4 aparecera un espacio vacio)
    else world[31] = 32;


    //Esta parte del código se encarga de hacer que los cactus se muevan hacia el dinosaurio, simulando que el dinosaurio está corriendo.
    // Recorre la segunda fila del mapa del juego
    for (int i = 16; i < 32; i++) {// con esto revivimos constantemente la segunda fila de la pantalla (16-31, ya sabemos que cada numero es un hueco en la pantalla LCD)
      // vemos si hay un cactus ( es 2 o es 3 ) para que se ejecute la siguiente parte del codigo 
      if (world[i] == 2 or world[i] == 3) {
        // vemos si la celda anterio era cactus o estaba vacia ( se guarda en anterior , hemos utilizado esta variable para que se entienda mejor )
        char anterior = (i < 31) ? world[i + 1] : 32;

        //vemos si el espacio justo antes del cactus está vacío. Si no está vacío, significa que el dinosaurio está ahí y ha chocado con el cactus. 
        //Si hay una colisión, el juego termina.
        if (world[i - 1] < 2) return 1;

        world[i - 1] = world[i];//Esta parte hace que el cactus se mueva un espacio hacia la izquierda.
        world[i] = anterior;//Aquí se llena el espacio que quedó vacío detrás del cactus con lo que había antes(cactus o vacio )
      }
    }

    
    world[15] = 32;//Esto asegura que la esquina superior derecha del mundo del juego siempre esté vacía.(si aparece algo , mal)

    // este if verifica si hay un cactus en la posición 16 del mundo del juego. 
    //Si hay un cactus ahí, significa que el dinosaurio acaba de saltar y dejar un espacio vacío. Entonces, se borra el cactus de esa posición(para que no haya fallos)
    if (world[16] < 2) world[16] = 32;

    return 0;// el juego continua ya que si se cumple la condicion significa que  no habria chocado el dinosaurio
  }



  void update_world() {  // hace que el cactus venga hacia el dinosaurio 
    int juego_terminado = scroll_world();// vemos si detecta colisiones o no (1 , seria jueo terminado ) se almacena en juego_terminado y si pasa eso salta a la linea de codigo de juego_terminado

    // en el caso en el que se gane el juego aparecera la siguiente linea de codigo 
    if (cuadrados == 250) {// se pone a 250 ya que es un valor alto y dificil  pero a la vez adsequible para que se pueda ganar algunas veces , se ha comprobado que se imprime bien en pantalla 
      lcd.setCursor(0, 0);
      lcd.print("   HAS GANADO!    ");
      lcd.setCursor(0, 1);
      lcd.write(byte(0));
      lcd.write(byte(32));
      lcd.write(byte(2));
      lcd.write(byte(2));
      lcd.write(byte(2));
      lcd.write(byte(3));
      lcd.write(byte(3));
      lcd.write(byte(3));
      lcd.write(byte(3));
      lcd.write(byte(3));
      lcd.write(byte(3));
      lcd.write(byte(2));
      lcd.write(byte(2));
      lcd.write(byte(2));
      lcd.write(byte(32));
      lcd.write(byte(1));
      while(1);
    }

    // se ha acabado el juego porque hemos chocado contra un cactus ( aparecera esto en pantalla )
    if (juego_terminado) {
      lcd.setCursor(0, 1);
      lcd.write(byte(0));// ya sabemos que el 0 representara un dinosaurio
      lcd.write(byte(3));// este reresentara un cactus 
      lcd.print("HAS PERDIDO!");
      lcd.write(byte(3));
      lcd.write(byte(0));
      while(1);
    }        



    // esta linea de codigo se utiliza para ir incrementando el numero de cuadrados que salen en pantalla 
     //Así  mostramos  en la pantalla cuánto cuadrados  ha recorrido .
    cuadrados++;
    
    // actualizamos los puntos ( el numero de cuadrados recorridos en esa partida )
    //lo mostramos en pantalla (puntos  es el nombre que se muestra ,indica el maximo de esa partida )
    lcd.setCursor(13, 0);
    lcd.print(cuadrados);

    lcd.setCursor(0, 0);// ponemos el cursor para empezar a poner cosas a partir de esa posicion 

    //------Bucle para ir posicionando el mundo ------------
    for (int i = 0; i < 32; i++) {// recorremos todas las posiciones de la pantalla con este bucle for 
      if (world[i] < 2) world[i] ^= 1;// con esto hacemos que el dinosaurio simule que camina levantando pierna izquierda y derecha simultaneamente 
      if (i == 16) lcd.setCursor(0, 1);// aqui actualizamos fila ya que al llegar al hueco 16 en la pantalla ya seria la segunda fila que tenemos 
      if (i < 13 || i > 15)// imprimimos en la pantalla lo que nos toque en esa posicion 
        lcd.write(byte(world[i]));
    }
  }


  // para el uso de los botones que implementamos en el juego 
  bool get_button() {
    // uso del boton up de la pantalla 
    #ifdef CMK_HARDWARE
      int entrada_pantalla;
     entrada_pantalla = analogRead (0);//Aquí se lee el valor analógico del pin 0 del Arduino. Esto se utiliza para leer los botones del shield LCD.
      if (entrada_pantalla < 200) return LOW;    // button up (vemos que si es menor , el boton esta siendo presionado )
      else return HIGH;
    #else
      return digitalRead(7);// esta linea " no hace falta" es para porsi se ejecuta con otro hardware
    #endif
  }



  //------------- arduino setup-----------
  void setup() {
    // Inicia la comunicación serial
    Serial.begin(9600);
    delay(1000); // Espera un segundo para asegurarse de que el Monitor Serial esté listo

    // Inicializa el LCD
    #ifndef CMK_HARDWARE
      EEPROM.put(eepromAddress, valorrecord);// ponemos el valorrecord en la posicion correspondiente de la eeprom 
      
      pinMode(7, INPUT_PULLUP);// por si no esta conectada la pantalla  LCD 
    #endif

    // -------asociamos numeros a objetos------------
    lcd.createChar(0, dino_l);// el numero 0 se asocia al dinosaurio con la pierna izquierda
    lcd.createChar(1, dino_r);// el numero 1 se asocia al dinosaurio con la pierna derecha 
    lcd.createChar(2, cactus_small);// numero 2 con cactus pequeño
    lcd.createChar(3, cactus_big);// numero 3 con el cactus grande 
    lcd.begin(16, 2);// inicializamos la pantalla con su tamaño correspondiente (16x2)

    
  }



  // -------------arduino loop-----------------
  void loop() {
    
    // para empezar a poner cosas en la pantalla desde esa posicion (inicial)
    lcd.setCursor(0, 0);

    int readValue; // inicializamos esta variable para leer el valor en la EEPROM del valor que le hemos puesto en esa casilla (RECORD DE CUADRADOS )
    readValue=EEPROM.read(eepromAddress);//leemos ese valor 

    lcd.print(" ESTAS LISTO? ");
    lcd.setCursor(1, 0);
    lcd.setCursor(0, 1);
    lcd.write(byte(0));
    lcd.write(byte(3));
    lcd.write(byte(2));
    lcd.print("RECORD:");
    lcd.print(readValue);//imprimimos ese valor de record de cuadrados en pantalla inicial 
    lcd.write(byte(2));
    lcd.write(byte(3));
    lcd.write(byte(0));
    
    while(get_button() == HIGH);

  
   
    // ----------------loop juego ---------------------
    // el while true inicia un bucle infinito.
    // Esto significa que el código dentro de este bucle se repetirá constantemente hasta que se apague el dispositivo o se reinicie el programa. 
    while(true) {
      // Para evitar que el dinosaurio se quede colgado en la fila superior todo el tiempo
      salto ^= 1;
      // cuando precioonamos el boton 
      if (get_button() == LOW && salto == true) {// si se encuentra  saltando se realiza lo de dentro del if 
        // actualizamos la posicion del dinosaurio en la pantalla
        lcd.setCursor(1, 1);
        lcd.write(byte(32));
        lcd.setCursor(1, 0);
        lcd.write(byte(0));

        // actualizamos la posicion del dinosaurio en el juego 
        world[1] = byte(0);
        world[17] = byte(32);

        // mientras el dinosaurio se "actualiza" para ver que tiene que salir despues
        for (int i = 0; i < 4; i++) update_world();

        // actualizamos la posicion del dinosaurio en el juego 
        world[1] = byte(32);
        world[17] = byte(0);

        // actualizamos la posicion del dinosaurio en la pantalla 
        lcd.setCursor(1, 0);
        lcd.write(byte(32));
        lcd.setCursor(1, 1);
        lcd.write(byte(0));
      }

      //------  para almacenar el valor del RECORD de cuadrados en la eeprom-----------------
       if (cuadrados > readValue) {// este if se hace para actualizar el valor en el caso de que sea mayor 
       readValue = cuadrados;// si es mayor pues ponemos ese valor 
       EEPROM.update(eepromAddress , readValue);// y actualizamos la eeprom con el nuevo valor  del record 
       } 
    
      //actualizamos el mundo para volver a iniciar una nueva partida en el juego 
      update_world();
    }
  }

