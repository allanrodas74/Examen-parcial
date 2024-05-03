#include <stdint.h>
#define RCC_C 0X40021000U //DECLARA A LOS REGISTROS DEL RELOJ

	#define PORTA 0x50000000U //DECLARA EL PUERTO A

	#define PORTB 0X50000400U //DECLARA EL PUERTO B

	#define PORTC 0X50000800U //DECLARAR EL PUERTO C

	#define PERIPHERAL_BASE_ADDRESS 0X40000000U  // BASE DE LOS PERIFERICOS

	#define AHB_BASE_ADDRESS (PERIPHERAL_BASE_ADDRESS + 0X00020000U) // BASE DEL AHB

	#define APB1_BASE_ADDRESS (PERIPHERAL_BASE_ADDRESS + 0X00000000U) // BASE DEL APB1

    #define USART2_BASE_ADDRESS ( APB1_BASE_ADDRESS + 0x00004400U) // USART2
    #define USART_ISR_RXNE (1 << 5)  // RXNE bit en el registro ISR del USART2
    #define USART_ISR_TXE (1 << 7) //

	//CREAR ESTRUCTURA DEL GPIO

	typedef struct
	{
			uint32_t MODER;
			uint32_t OTYPER;
			uint32_t OSPEEDR;
			uint32_t PUPDR;
			uint32_t IDR;
			uint32_t ODR;
			uint32_t BSRR;
			uint32_t LCKR;
			uint32_t AFR[2];
			uint32_t BRR;

	} GPIO_RegDef_t;

	//CREAR ESTRUCTURA DEL RCC

	typedef struct{
			uint32_t CR;
			uint32_t ICSCR;
			uint32_t CRRCR;
			uint32_t CFGR;
			uint32_t CIER;
			uint32_t CIFR;
			uint32_t CICR;
			uint32_t IOPRSTR;
			uint32_t AHBRSTR;
			uint32_t APB2RSTR;
			uint32_t APB1RSTR;
			uint32_t IOPENR;
			uint32_t AHBENR;
			uint32_t APB2ENR;
			uint32_t APB1ENR;
			uint32_t IOPSMENR;
			uint32_t AHBSMENR;
			uint32_t APB2SMENR;
			uint32_t APB1SMENR;
			uint32_t CCIPR;
			uint32_t CSR;
	}RCC_RegDef_t;

	typedef struct{
		uint32_t CR1;
		uint32_t CR2;
		uint32_t CR3;
		uint32_t BRR;
		uint32_t GTPR;
		uint32_t RTOR;
		uint32_t RQR;
		uint32_t ISR;
		uint32_t ICR;
		uint32_t RDR;
		uint32_t TDR;
	}USART_RegDef_t;

	// ESTRUCTURA PARA LLEVAR CONTROL DE VARIBALES DE TIEMPO
	struct Time_t {

		int8_t hora_dec;
		int8_t hora_uni ;

		int8_t minuto_dec ;
		int8_t minuto_uni  ;

		int8_t segundo_dec ;
		int8_t segundo_uni ;


	};
	struct alarm {

		uint8_t hora_dec;
		uint8_t hora_uni ;

		uint8_t minuto_dec ;
		uint8_t minuto_uni  ;

		uint8_t segundo_dec ;
		uint8_t segundo_uni ;


	};
	struct temporizador{
			uint8_t hora_dec ;
			uint8_t hora_uni ;

			uint8_t minuto_dec ;
			uint8_t minuto_uni ;

			uint8_t segundo_dec ;
			uint8_t segundo_uni ;
	};

	#define GPIOA 	((GPIO_RegDef_t*)PORTA)
	#define GPIOB 	((GPIO_RegDef_t*)PORTB)
	#define GPIOC 	((GPIO_RegDef_t*)PORTC)
	#define RCC 	((RCC_RegDef_t*)RCC_C)
	#define USART2 	((USART_RegDef_t*)USART2_BASE_ADDRESS)


	uint8_t myfsm = 0x00; //CREANDO VARIABLE QUE CONTROLA LA SECUENCIA DE ENCEDIDO DE LOS DISPLAY
	uint8_t myfsm1 = 0x00; // CREANDO VARIABLER QUE CONTROLA LA MAQUINA DE ESTAOS DINITOS DEL KPAD
	uint16_t mascara = 0x00; //MASCARA PARA SOLO USAR LOS ULTIMOS 4 BITS DEL GPIOB

	int numeros;
	int numero_1[2] = {0,0};  //ARREGLO QUE ALAMCENA DOS DIGITOS
	int numero_2[2] = {0,0};  //ARREGLO QUE ALAMCENA DOS DIGITOS
	int contadores = 0; // CUENTA LOS DIGITOS ALMACENADOS EN EL ARREGLO
	int numero_enteros; // NUMERO ENTERO CONVERSION DEL ARREGLO NUMERO_UNO
	int numero_enteros_dos; //ALMACENA EL SIGUENTE DATO DEL OPERADOR
	char operadores; // OPERACION QUE SE REALIZARA
	int caso;  // VERIFICA QUE OPERACION SE ESTA REALIZANDO PARA OPERAR
	int resultado; // ALAMACENA EL DATO DE LA OPERACION
	int ejecutado =0;
	int estado = 0;
	int tecla =0;

	int Serial_Keypad = 0; //CAMBIA ENTRE ESTADO DE SERIAL Y RELOJ

	uint32_t inc_segundos;
	void delay_ms(uint16_t n); //DEFINDIENDO EL METODO DE RETARDO

	uint8_t decoder(uint8_t value_to_decode); //DEFINIDENDO LA FUNCION SWITCH

	//DIGITOS EN CATODO COMUN

	#define digit0 0x3F
	#define digit1 0X06
	#define digit2 0X5B
	#define digit3 0X4F
	#define digit4 0X66
	#define digit5 0X6D
	#define digit6 0X7D
	#define digit7 0X07
	#define digit8 0X7F
	#define digit9 0X6F
	#define A 0X77
	#define B 0X7C
	#define C 0X39
	#define D 0X5E


	int main(void)
	{

	    char opciones;
	    int numero1, numero2;
		struct Time_t watch;

		watch.segundo_uni = 0x00;
		watch.segundo_dec = 0x00;
		watch.minuto_uni = 0x00;
		watch.minuto_dec = 0x00;
		watch.hora_uni = 0x00;
		watch.hora_dec = 0x00;




		//CONFIGURACION DE LOS PINES DEL PUERTO A



		RCC ->IOPENR |= 1 << 0; //HABILITAR PUERTO A RELOJ


		//CONFIGURACIONES DEL PIN A0 PARA CAMBIAR ENTRE ESTADO DE SERIAL Y KPAD

		GPIOA ->MODER &= ~(1 << 0); //PIN A0
		GPIOA ->MODER &= ~(1 << 1);
		GPIOA ->PUPDR &= ~(1 << 0); //RESISTENCIA DE PULLDOWN
		GPIOA ->PUPDR |= 1 << 1;
		// para 16Mhz
				//RCC ->CR |=(1<<0);
				//configuracion GSI16
				//RCC ->CFGR |= (1<<0);
			    // PA5
			    //GPIOA->MODER &= ~(1 << 11);

			    // CONFIGURACION USART 2
			    // alternar la funcion modo  para PA2 y PA3
			    GPIOA->MODER &= ~(1 << 4);  //[10]
			    GPIOA->MODER &= ~(1 << 6);
			    // seleccionar cual moder AF4 para usart
			    GPIOA->AFR[0] |= 1 << 10;  // mapera PA2 a AF4
			    GPIOA->AFR[0] |= 1 << 14;  // mapear PA3 a AF4

			    // configuracion periferico del USART2
			    // Clock enable RCC APB1ENR resgistro
			    RCC->APB1ENR |= 1 << 17;
			    // la tasa de los bits sera expulsado   9600 con 8Mhz
			    USART2->BRR =218;
			    // habilitamos trasmisor y receptor
			    USART2->CR1 |= (1 << 2) | (1 << 3);
			    // habilitar el periferico
			    USART2->CR1 |= 1 << 0;



		//CONFIGURACION DE PINES DEL PUERTO B
		RCC -> IOPENR |= 1 << 1; //HABILITAR  PUERTO B RELOJ

		// PINES CONFIGURADOS COMO SALIDA DEL PUERTO B, DISPLAY DE 7 SEGMENTOS MULTIPLEXADO

		GPIOB ->MODER |= 1<<0;  //PIN B0
		GPIOB ->MODER &= ~(1 << 1);

		GPIOB ->MODER |= 1<< 2;	//PIN B1
		GPIOB ->MODER &= ~(1 << 3);

		GPIOB ->MODER |= 1 << 4;	//PIN B2
		GPIOB ->MODER &= ~(1 << 5);

		GPIOB ->MODER |= 1 << 6;	//PIN B3
		GPIOB ->MODER &= ~(1 << 7);

		GPIOB ->MODER |= 1 << 8;	//PIN B4
		GPIOB ->MODER &= ~(1 << 9);

		GPIOB ->MODER |= 1 << 10;	//PIN B5
		GPIOB ->MODER &= ~(1 << 11);

		GPIOB ->MODER |= 1 << 12;	//PIN B6
		GPIOB ->MODER &= ~(1 << 13);

		GPIOB ->MODER |= 1 << 14;	//PIN B7
		GPIOB ->MODER &= ~(1 << 15);

		// PINES CONFIGURADOS COMO ENTRADA DEL PUERTO B DEL KPAD

		GPIOB ->MODER &= ~(1 << 16); // PIN PB8
		GPIOB ->MODER &= ~(1 << 17);

		GPIOB ->PUPDR |= 1<<16; //RESISTENCIA DE PULL-UP PB8
		GPIOB ->PUPDR &= ~(1 << 17);

		GPIOB ->MODER &= ~(1 << 18); //PIN PB9
		GPIOB ->MODER &= ~(1 << 19);

		GPIOB ->PUPDR |= 1<<18; //RESISTENCIA DE PULL-UP PB9
		GPIOB ->PUPDR &= ~(1 << 19);

		GPIOB ->MODER &= ~(1 << 20); //PIN PB10
		GPIOB ->MODER &= ~(1 << 21);

		GPIOB ->PUPDR |= 1<<20; //RESISTENCIA DE PULL-UP PB10
		GPIOB ->PUPDR &= ~(1 << 21);

		GPIOB ->MODER &= ~(1 << 22); // PB11
		GPIOB ->MODER &= ~(1 << 23);

		GPIOB ->PUPDR |= 1<<22; //RESISTENCIA DE PULL-UP PB11
		GPIOB ->PUPDR &= ~(1 << 23);

		// PINES CONFIGURADOS COMO SALIDA DEL PUERTO B DEL KPAD

		GPIOB ->MODER |= 1 << 24;	//PIN B12
		GPIOB ->MODER &= ~(1 << 25);

		GPIOB ->MODER |= 1 << 26;	//PIN B13
		GPIOB ->MODER &= ~(1 << 27);

		GPIOB ->MODER |= 1 << 28;	//PIN B14
		GPIOB ->MODER &= ~(1 << 29);

		GPIOB ->MODER |= 1 << 30;	//PIN B15
		GPIOB ->MODER &= ~(1 << 31);

	   //CONFIGURACIONES PARA EL PUERTO C, CONTROL DE DISPLAY MULTIPLEXADO

		RCC ->IOPENR |= 1 << 2; //HABILITAR PUERTO C RELOJ

		//PINES CONFIGURADOS COMO SALIDA DEL PUERTO C

		// PINES DE CONTROL DEL DISPLAY MULTIPLEXADO

		GPIOC ->MODER |= 1 << 10;  //PIN C5
		GPIOC ->MODER &= ~(1 << 11);

		GPIOC ->MODER |= 1 << 12;  //PIN C6
		GPIOC ->MODER &= ~(1 << 13);

		GPIOC ->MODER |= 1 << 16;  //PIN C8
		GPIOC ->MODER &= ~(1 << 17);

		GPIOC ->MODER |= 1 << 18;  //PIN C9
		GPIOC ->MODER &= ~(1 << 19);

		GPIOC ->MODER |= 1 <<20;//PIN C10
		GPIOC ->MODER &= ~(1 << 21);

		GPIOC ->MODER |= 1 <<22;//PIN C11
		GPIOC ->MODER &= ~(1 << 23);





		while(1)
		{




			//MANEJO DEL DISPLAY
			if(Serial_Keypad == 1){

				myfsm = 0;
			}
			switch(myfsm)
			{
			case 0:
			{
				//PARA EL DISPLAY 1 UNIDAD SEGUNDOS
				if (Serial_Keypad == 0){
					myfsm++;

				}
				else if (Serial_Keypad == 1){
					myfsm =0;
				}
				GPIOB ->BSRR |= 0XFF << 16; //LIMPIAR DISPLAY
				GPIOC ->BSRR |= 0X01 << 27; // LIMPIAR D5/C11
				GPIOC ->BSRR |= 0X01 << 5; // HABILITA D0/C5
				GPIOB ->BSRR |= decoder(watch.segundo_uni) << 0; // MANDA A PINTAR DISPLAY 1



				break;

			}
			case 1:{
				//PARA EL DISPLAY 2 DECENA SEGUNDOS
				if (Serial_Keypad == 0){
					myfsm++;
					GPIOB ->BSRR |= 0XFF << 16; //LIMPIAR DISPLAY
					GPIOC ->BSRR |= 0X01 << 21; // LIMPIAR D0/C5
					GPIOC ->BSRR |= 0X01 << 6 ; // HABILITA D1/C6
					GPIOB ->BSRR |= decoder(watch.segundo_dec) << 0; //MANDA A PINTAR DISPLAY 2
				}
				else if (Serial_Keypad == 1){
					myfsm =0;
				}

				break;

			}
			case 2:{
				//PARA EL DISPLAY 3 UNIDAD MINUTOS
				if (Serial_Keypad == 0){
					myfsm++;
					GPIOB ->BSRR |= 0XFF << 16; //LIMPIAR DISPLAY
					GPIOC ->BSRR |= 0X01 << 22; // LIMPIAR D1/C6
					GPIOC ->BSRR |= 0X01 << 8; // HABILITA D2/C8
					GPIOB ->BSRR |= decoder(watch.minuto_uni) << 0; //MANDA A PINTAR DISPLAY 3
				}
				else if (Serial_Keypad == 1){
					myfsm =0;
				}



				break;
			}
			case 3:{
				//PARA EL DISPLAY 4 DECENA MINUTOS

				if (Serial_Keypad == 0){
					myfsm++;
					GPIOB ->BSRR |= 0XFF << 16; //LIMPIAR DISPLAY
					GPIOC ->BSRR |= 0X01 << 24; // LIMPIAR D2/C8
					GPIOC ->BSRR |= 0X01 << 9; // HABILITA D3/C9
					GPIOB ->BSRR |= decoder(watch.minuto_dec) << 0; //MANDA A PINTAR DISPLAY 4
				}
				else if (Serial_Keypad == 1){
					myfsm =0;
				}

				break;

			}
			case 4:{
				//PARA EL DISPLAY 5 UNIDAD HORAS
				if (Serial_Keypad == 0){
					myfsm++;
					GPIOB ->BSRR |= 0XFF << 16; //LIMPIAR DISPLAY
					GPIOC ->BSRR |= 0X01 << 25; // LIMPIAR D3/C9
					GPIOC ->BSRR |= 0X01 << 10; // HABILITA D4/C10
					GPIOB ->BSRR |= decoder(watch.hora_uni) << 0; //MANDA A PINTAR DISPLAY 4
				}
				else if (Serial_Keypad == 1){
					myfsm =0;
				}


				break;

					}

			case 5:{
				//PARA EL DISPLAY 6 DECENA HORAS
				if (Serial_Keypad == 0){
					myfsm =0;
					GPIOB ->BSRR |= 0XFF << 16; //LIMPIAR DISPLAY
					GPIOC ->BSRR |= 0X01 << 26 ; // LIMPIAR D4/C10
					GPIOC ->BSRR |= 0X01 << 11; // HABILITA D5/C11
					GPIOB ->BSRR |= decoder(watch.hora_dec) << 0; //MANDA A PINTAR DISPLAY 4
				}
				else if (Serial_Keypad == 1){
					myfsm =0;
				}



				break;

							}

			default:
			{
				//CASO NO TOMA EN CUENTA NINGUNO myfsm POR SEGURIDAD
				myfsm =0 ;
				break;
			}

			}
			//TERMINA MAQUINA DE ESTADOS FINITOS DISPLAY


			//EMPIEZA DISPLAY KPAD
			if (Serial_Keypad==0){

			switch(myfsm1)
			{
			case 0:{
				//PARA LA COLUMAN 0
				myfsm1++;
				delay_ms(1);
				GPIOB->ODR |= 1<<15; // APAGANDO PB15
				GPIOB->ODR &= ~(1 << 12); //HABILITANDO PB12
				mascara = GPIOB->IDR & 0XF00; //CREA MASCARA DE 1111-0000-0000
				if (mascara == 0xE00){ //1110
					operadores = '+';  //SUMA LETRA A
					numeros = 100;
					caso =1;
					tecla =0;

				}
				else if (mascara == 0xD00 ){ //1101
					operadores = '-'; //RESTA LETRA B
					numeros =100;
					caso =2;
					tecla =0;
				}
				else if(mascara == 0xB00){ //1011
					operadores = '*'; //MULTIPLICACION LETRA C
					numeros =100;
					caso =3;
					tecla =0;
				}
				else if (mascara ==0X700){ //0111
					operadores = '='; //RESULTADO LETRA D
					tecla =0;


				}



				break;

			}
			case 1:{
				//PARA COLUMNA 1
				myfsm1++;
				GPIOB->ODR |= 1<<12; // APAGANDO PB12
				GPIOB->ODR &= ~(1 << 13); //HABILITANDO PB13
				mascara = GPIOB->IDR & 0XF00; //CREA MASCARA DE 1111-0000-0000
				if (mascara == 0xE00){ //1110
					numeros = 3; //NUMERO 3
					operadores = '/';
					tecla =1;
				}
				else if(mascara == 0xD00 ){ //1101
					 //NUMERO 6
					numeros = 6;
					operadores = '/';
					tecla =1;
				}
				else if (mascara == 0xB00){ //1011
					 // NUMERO 9
					numeros = 9;
					operadores = '/';
					tecla =1;
				}
				else if (mascara ==0X700){ //0111
					 //NUM
					Serial_Keypad = 1;
				}

				break;
			}
			case 2:{
				//PARA COLUMNA 2
				myfsm1++;
				GPIOB->ODR |= 1<<13; // APAGANDO PB13
				GPIOB->ODR &= ~(1 << 14); //HABILITANDO PB14
				mascara = GPIOB->IDR & 0XF00; //CREA MASCARA DE 1111-0000-0000
				if (mascara == 0xE00){ //1110
					 //NUMERO 2
					numeros = 2;
					operadores = '/';
					tecla =1;


				}
				else if (mascara == 0xD00 ){ //1101
					 //NUMERO 5
					numeros = 5;
					operadores = '/';
					tecla =1;


				}
				else if (mascara == 0xB00){ //1011
					 //NUMERO 8
					numeros = 8;
					operadores = '/';
					tecla =1;


				}
				else if (mascara ==0X700){ //0111
					 //NUMERO 0
					numeros =0;
					operadores = '/';
					tecla =1;


				}

				break;
			}

			case 3:{
				//PARA COLUMNA 3
				myfsm1 = 0;

				GPIOB->ODR |= 1<<14; // APAGANDO PB14
				GPIOB->ODR &= ~(1 << 15); //HABILITANDO PB15
				mascara = GPIOB->IDR & 0XF00; //CREA MASCARA DE 1111-0000-0000
				if (mascara == 0xE00){ //1110
					 //NUMERO 1
					numeros = 1;
					tecla =1;
					operadores = '/';


				}
				else if (mascara == 0xD00 ){ //1101
					 //NUMERO 4
					numeros =4;
					tecla =1;
					operadores = '/';

				}
				else if(mascara == 0xB00){ //1011
					 //NUMERO 7
					numeros =7;
					operadores = '/';
					tecla =1;
				}
				else if(mascara ==0X700){ //0111
					 //NUMERO *
					contadores =0;
					caso =0;
					estado =0;
					numeros =100;
					numero_1[0]=0;
					numero_1[1]=0;
					numero_2[0]=0;
					numero_2[1]=0;
					operadores ='/';
					watch.segundo_dec =0;
					watch.segundo_uni =0;

				}


				break;
			}
			default:{
				myfsm1 = 0;

				break;
			}



			} //TERMINA MAQUINA DE ESTADOS FINITOS KPAD

			//EMPIEZA CALCULADORA


			if(mascara != 0XF00 && tecla ==1 ){ //ALMACENA EL NUMERO PRESIONADO
				delay_ms(500);
				if(estado ==0 ){
					if(numeros >=0 && numeros <=9){
						numero_1[contadores] = numeros;

						watch.segundo_dec = numero_1[0];
						watch.segundo_uni = numero_1[1];
						contadores++; //AUMENTA PARA ALMACENAR SIGUIENTE NUMERO PRESIONADO
					}
				}
				if(estado ==1){
					if(numeros >=0 && numeros <=9){
						numero_2[contadores] = numeros;

						watch.segundo_dec = numero_2[0];
						watch.segundo_uni = numero_1[1];
						contadores++; //AUMENTA PARA ALMACENAR SIGUIENTE NUMERO PRESIONADO
					}
				}

				if(contadores >1 ){
					contadores = 0;
					}
				}

			switch(operadores){
				case '+':

					numero_enteros =numero_1[0] * 10 + numero_1[1]; // CONVERTIR ARREGLO EN NUMERO ENTERO
					contadores = 0;
					caso =1;
					estado =1;
					watch.segundo_dec =0;
					watch.segundo_uni =0;
					operadores ='/';


					break;
				case '-':

					numero_enteros =numero_1[0] * 10 + numero_1[1]; // CONVERTIR ARREGLO EN NUMERO ENTERO
					contadores = 0;
					caso =2;
					estado =1;
					watch.segundo_dec =0;
					watch.segundo_uni =0;
					operadores ='/';
					break;
				case '*':
					numero_enteros =numero_1[0] * 10 + numero_1[1]; // CONVERTIR ARREGLO EN NUMERO ENTERO
					contadores = 0;
					caso =3;
					estado =1;
					watch.segundo_dec =0;
					watch.segundo_uni =0;
					operadores ='/';

					break;
				case '=':
					if (caso==1){

						numero_enteros_dos =numero_2[0] * 10 + numero_2[1];
						resultado =numero_enteros + numero_enteros_dos;
						if(resultado >=0 && resultado <=99){
							int partentera = resultado /10;
							int partdecimal = resultado %10;

							watch.segundo_uni = partdecimal ;
							watch.segundo_dec = partentera ;
							contadores =0;
							caso =0;
							estado =0;
							numeros =100;
							numero_1[0]=0;
							numero_1[1]=0;
							numero_2[0]=0;
							numero_2[1]=0;
							operadores ='/';
						}
						else {
						watch.segundo_uni = 0 ;
						watch.segundo_dec = 0 ;
						contadores =0;
						caso =0;
						estado =0;
						numeros =100;
						numero_1[0]=0;
						numero_1[1]=0;
						numero_2[0]=0;
						numero_2[1]=0;
						operadores ='/';
						}
					}
					if(caso ==2){
						numero_enteros_dos =numero_2[0] * 10 + numero_2[1];
						resultado =numero_enteros - numero_enteros_dos;
						if(resultado >=0 && resultado <=99){
						int partentera = resultado /10;
						int partdecimal = resultado %10;

						watch.segundo_uni = partdecimal ;
						watch.segundo_dec = partentera ;
						caso =0;
						estado =0;
						numeros =100;
						numero_1[0]=0;
						numero_1[1]=0;
						numero_2[0]=0;
						numero_2[1]=0;
						operadores ='/';
						}
						else {
							watch.segundo_uni = 0 ;
							watch.segundo_dec = 0 ;
							caso =0;
							estado =0;
							numeros =100;
							numero_1[0]=0;
							numero_1[1]=0;
							numero_2[0]=0;
							numero_2[1]=0;
							operadores ='/';

						}
					}
					if(caso==3){
						numero_enteros_dos =numero_2[0] * 10 + numero_1[1];
						resultado =numero_enteros * numero_enteros_dos;
						if(resultado >=0 && resultado <=99){
						int partentera = resultado /10;
						int partdecimal = resultado %10;

						watch.segundo_uni = partdecimal ;
						watch.segundo_dec = partentera ;
						caso =0;
						estado =0;
						numeros =100;
						numero_1[0]=0;
						numero_1[1]=0;
						numero_2[0]=0;
						numero_2[1]=0;
						operadores ='/';
						}
						else{
							watch.segundo_uni = 0;
							watch.segundo_dec = 0 ;
							caso =0;
							estado =0;
							numeros =100;
							numero_1[0]=0;
							numero_1[1]=0;
							numero_2[0]=0;
							numero_2[1]=0;
							operadores ='/';


						}

					}

				//TERMINA CALCULADORA




			}
			}// Serial_Kpad = 0

			//MODO SERIAL

			if(Serial_Keypad == 1){
				watch.segundo_dec =0;
				watch.segundo_uni =0;
			void UART_SendChar(char c) {
			    while (!(USART2->ISR & USART_ISR_TXE)); // Esperar a que el registro de transmisión esté vacío
			    USART2->TDR = c; // Enviar el carácter
			}

			// Función para recibir un carácter por la UART
			char UART_ReceiveChar() {
			    while (!(USART2->ISR & USART_ISR_RXNE)); // Esperar a que llegue un byte
			    return USART2->RDR; // Devolver el byte recibido
			}

			// Función para enviar una cadena de caracteres por la UART
			void UART_SendString(const char *str) {
			    while (*str != '\0') {
			        UART_SendChar(*str++);
			    }
			}

			// Función para imprimir el menú de operaciones
			void printMenu() {
			    UART_SendString("---- Menu de Operaciones ----\r\n");
			    UART_SendString("1. Suma\r\n");
			    UART_SendString("2. Resta\r\n");
			    UART_SendString("3. Multiplicacion\r\n");
			    UART_SendString("4. Division\r\n");
			    UART_SendString("Seleccione una opcion: ");
			}

			// Imprimir el menú
			        printMenu();

			        opciones = UART_ReceiveChar();
			        UART_SendChar(opciones); // Mostrar la opción seleccionada
			        int readTwoDigitNumber() {
			            int firstDigit = UART_ReceiveChar() - '0'; // Convertir el primer carácter a número
			            UART_SendChar(firstDigit + '0'); // Mostrar el primer dígito ingresado
			            int secondDigit = UART_ReceiveChar() - '0'; // Convertir el segundo carácter a número
			            UART_SendChar(secondDigit + '0'); // Mostrar el segundo dígito ingresado

			            return firstDigit * 10 + secondDigit; // Combinar los dos dígitos en un número de dos dígitos
			        }
			        UART_SendString("\r\nIngrese el primer numero: ");
			        numero1 = readTwoDigitNumber(); // Leer el primer número de dos dígitos

			        UART_SendString("\r\nIngrese el segundo numero: ");
			        numero2 = readTwoDigitNumber(); // Leer el segundo número de dos dígitos



			        int result;
			        switch (opciones) {
			            case '1': // SUMA
			                result = numero1 + numero2;
			                break;
			            case '2': // RESTA
			                result = numero1 - numero2;
			                break;
			            case '3': // MULTIPLICACION
			                result = numero1 * numero2;
			                break;
			            case '4': // DIVISION
			                if (numero2 != 0) {
			                    result = numero1 / numero2;
			                } else {
			                    result = 0; // MANEJAR LA DIVISION
			                }
			                break;
			            default:
			                result = 0; // Opción inválida
			                break;
			        }


			        watch.segundo_uni = result %10 ; // Obtiene el dígito de las unidades
			        watch.segundo_dec=result /10;;


			        UART_SendString("\r\nEl resultado es: ");
			        char buffer[10]; // Se reserva un espacio para el valor máximo de un uint16_t (5 dígitos) y el carácter nulo final
			        sprintf(buffer, "%02u\n", (unsigned int)result); // formatea e imprime una serie de caracteres y valores en la corriente de salida estándar stdout
			       	UART_SendString(buffer); // Envía el resultado formateado

			        UART_SendString("\r\n desea pasar al kpad presione: 0");
			        UART_SendString("\r\n desea usar la consola serial nuevamente presione: 1");
			        UART_SendString("\r\n ");
			        Serial_Keypad = UART_ReceiveChar() - '0'; // Convertir el carácter a número
			        if(Serial_Keypad==0){
			        	watch.segundo_uni =0;
			        	watch.segundo_dec=0;
			        }









			    }
		}

		//TERMINA EL WHILE

	}






	void delay_ms(uint16_t n){
// para 2.097 140
		// para 16MHZ 3195
		uint32_t i;
		for(; n>0; n--)
			for(i=0; i<140; i++);
	}
	uint8_t decoder(uint8_t value_to_decode)
	{
		switch(value_to_decode)
		{
		case 0:
		{
			return(digit0);

		}
		case 1:{

			return(digit1);
		}
		case 2:{

			return(digit2);
		}
		case 3:{

			return(digit3);
		}
		case 4:{

			return(digit4);
		}
		case 5:{

			return(digit5);
		}

		case 6:{

			return(digit6);
		}

		case 7:{

			return(digit7);
		}

		case 8:{

			return(digit8);
		}
		case 9:{

			return(digit9);
		}
		case 10:{
			return(A);
		}
		case 11:{
			return(B);
		}
		case 12:{
			return(C);
		}
		case 13:{
			return(D);
		}


		default:
		{

		}
		return 0x00;


		}
	}

