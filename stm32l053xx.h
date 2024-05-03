

#ifndef STM32L053XX_H_
#define STM320L053xx_H_H

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



#endif /* STM32L053XX_H_ */
