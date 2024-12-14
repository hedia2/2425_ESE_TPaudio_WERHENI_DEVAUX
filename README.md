# 2425_ESE_TPaudio_WERHENI_DEVAUX

## 1.Démarrage
## Testez la LED LD2

La fonction HAL_GPIO_TogglePin() inverse l'état logique de la broche associée à la LED (passant de l'état haut à bas, ou inversement), ce qui allume ou éteint la LED. Ensuite, la fonction HAL_Delay(500) introduit un délai de 500 millisecondes pour créer une pause visible avant de répéter l'opération. 

![image](https://github.com/user-attachments/assets/32837cbc-2ad0-4117-967e-1fc069c35f44)

## Testez l’USART2 connecté à la STLink interne.

![image](https://github.com/user-attachments/assets/7bcba097-c0d1-4368-b70a-1f43a23528af)

Nous allons utulisé :
* __io_putchar() : Redirige les sorties de printf() vers l'UART, permettant d'envoyer des caractères un par un.
* HAL_UART_Transmit() avec délai : Envoie un message ou des données via l'UART, suivi d'une pause, pour des transmissions régulières ou périodiques.


![image](https://github.com/user-attachments/assets/f66172ae-f3fb-43ac-983b-8ba6c4e167b1)

## Activez FreeRTOS en mode CMSIS V1.

FreeRTOS, un système d'exploitation temps réel, est activé et peut être personnalisé via des options comme les tâches, sémaphores et mutexes. L'interface facilite la configuration matérielle et logicielle avant de générer le code d'initialisation pour le projet.

![image](https://github.com/user-attachments/assets/bac0f2a7-7444-40aa-9f6c-9b0098be24a7)

## Faites fonctionner le shell :
## (a) Dans une tâche
Nous allons définir une fonction de tâche FreeRTOS appelée TAskFonction, qui est conçue pour s'exécuter en boucle infinie (for (;;)). À chaque itération, elle appelle la fonction shell_run(&h_shell), qui semble gérer l'exécution d'un shell ou d'une interface de commande interactive. Cette tâche est typiquement utilisée pour permettre un fonctionnement continu du shell dans un système temps réel.

![image](https://github.com/user-attachments/assets/950b6061-a6b1-4969-accd-1bb7749f3c25)

## (b) En mode interruption (version branch Heidia2)

Nous allons utulisé Un sémaphore de type FreeRTOS (sem_usart1) est utilisé pour signaler à une tâche en attente que la réception des données est terminée. Ceci est réalisé grâce à la fonction xSemaphoreGiveFromISR, qui est spécialement conçue pour être utilisée dans un contexte d'interruption.

![image](https://github.com/user-attachments/assets/fed54be8-5fe4-4ec9-976d-d4f8faa341c6)

![image](https://github.com/user-attachments/assets/945f865d-7bf4-49c7-84a5-5e730f569b08)


## (c) Avec un driver sous forme de structure (version branch main)

On utilise un driver pour faire marché le shell, plus particulierement pour la transmission et la réception de donnée par UART (UART2 intégré à la ST-LINK). Ci-dessous les fonction principales du driver (vous pouvez retrouver le code complet dans la paire de fichier drv_uart1.h/drv_uart1.c):
```
uint8_t drv_uart2_receive(char * pData, uint16_t size);
uint8_t drv_uart2_transmit(const char * pData, uint16_t size);
```
On utilise également une structure de donnée pour l'utilisation du shell (dans laquelle est défini notament les données a envoyées et reçues). Le code complet de la struture ` h_shell_struct;` est disponible dans le fichier `shell.h`.

## 2.Le GPIO Expander et le VU-Metre

## 2.1 Configuration

## la référence du GPIO Expander
Le MCP23S17 est un expander GPIO 16 bits qui communique avec un microcontrôleur via une interface SPI. Il permet d'ajouter des entrées/sorties numériques supplémentaires au microcontrôleur lorsqu'il manque de GPIO disponibles. Voici un résumé de ses principales caractéristiques :

1.Interface SPI : Il utilise une communication série rapide pour échanger des données avec le microcontrôleur.
2.16 broches de GPIO : Les broches peuvent être configurées individuellement comme entrées ou sorties.
3.Configuration par registres : Vous devez écrire dans des registres spécifiques, tels que IODIRA et IODIRB, pour définir le mode (entrée/sortie) des broches.

## SPI de STM32

Nous allons utiliser l'interface SPI3 de la carte STM32 pour communiquer avec le composant MCP23S17. Les connexions des broches sont établies comme suit :

*MISO (Master In Slave Out) est connecté à la broche PC11 de la carte STM32.

*SCK (horloge SPI) est connecté à la broche PC10.

*MOSI (Master Out Slave In) est connecté à la broche PB5.

*CS (Chip Select) est connecté à la broche PB7.

*RESET est connecté à la broche PA0.

Ces connexions permettent d'assurer la communication SPI entre le microcontrôleur et l'expander GPIO, en respectant les lignes de données et de contrôle nécessaires pour le bon fonctionnement du protocole SPI.

![image](https://github.com/user-attachments/assets/741566ed-9f08-4424-b18b-c943a18c29df)

## les paramètres à configurer dans STM32CubeIDE 

L'image ci-dessous montre la configuration de l'interface SPI3 pour la carte STM32L476RGTx dans STM32CubeMX. Le mode SPI3 est configuré en tant que maître full-duplex avec le signal NSS (Chip Select) désactivé.

*Le frame format est défini sur Motorola, et le data size est de 8 bits.

*Le premier bit est transmis MSB First (Most Significant Bit First).

*Pour la configuration de l'horloge, la fréquence de prescaler est fixée à 16, et la vitesse de transmission est de 5.0 Mbits/s.

*La polarité de l'horloge (CPOL) est Low, et la phase de l'horloge (CPHA) est définie sur 1 Edge.


![image](https://github.com/user-attachments/assets/d33c21e2-c221-485b-920e-ed8516f2750a)

## 2.2 Tests

## 1.Faites clignoter une ou plusieurs LED
Nous allons déclaré la fonction MCP23S17_Init() qui permet de :

*renitialisation de pin Reset 

*La commande MCP23S17_Write(0x00, 0x00); configure tous les pins du MCP23S17 comme sorties. Elle écrit la valeur 0x00 dans le registre IODIRA, ce qui définit chaque pin en mode sortie. 

*La commande MCP23S17_Write(0x01, 0x00); écrit la valeur 0x00 dans le registre IODIRB du MCP23S17. Le registre IODIRB contrôle la direction des pins du port B (par opposition au port A contrôlé par le registre IODIRA). En écrivant 0x00, elle configure toutes les pins du port B du MCP23S17 en mode sortie.

aprés Nous allons déclaré la fonction MCP23S17_Write permet d'envoyer une commande d'écriture via SPI pour configurer un registre du MCP23S17. Elle commence par préparer un tableau contenant le code d'écriture, l'adresse du registre et les données à écrire. Ensuite, elle active le signal Chip Select (CS) en le mettant à l'état bas, puis transmet les données via la fonction HAL_SPI_Transmit. 

## 2.un chenillard 

```
void chenillard(void) {
    // Supposons que vous avez 8 LEDs (A0 à A7 sur le MCP23S17)
    for (int i = 0; i < 8; i++) {
        // Allumer la LED correspondante (i)
        MCP23S17_Write(0x12, ~(1 << i)); // Éteindre toutes les autres LEDs
        MCP23S17_Write(0x13, ~(1 << i));
        printf("LED %d allumée\r\n", i + 1);
        HAL_Delay(100); // Attendre 100 ms

        // Éteindre la LED actuelle avant de passer à la suivante
        MCP23S17_Write(0x12, 0xFF); // Éteindre toutes les LEDs
    }
}
```
1.Une boucle for est utilisée pour parcourir les indices de 0 à 7, correspondant aux 8 LEDs.

2.À chaque itération :
-Une LED spécifique est allumée en écrivant un masque binaire inversé ~(1 << i) dans les registres 0x12 et 0x13 du MCP23S17, ce qui assure que seule la LED de l'indice courant s'allume, tandis que les autres sont éteintes.

-Un message est affiché dans la console (printf) pour indiquer quelle LED est actuellement allumée.

-La fonction attend 100 millisecondes (HAL_Delay(100)) avant de passer à l'étape suivante.

3.Après avoir allumé la LED, la fonction éteint toutes les LEDs en écrivant la valeur 0xFF dans le registre 0x12 avant de passer à l'itération suivante. Cela garantit qu'aucune LED ne reste allumée lorsqu'une nouvelle s'allume.

Ce processus continue jusqu'à ce que toutes les LEDs aient été successivement allumées,

## 3 Le CODEC Audio SGTL5000

### 3.# Configuration préalables
Commençons par configurer l'I2C et l'I2S dans l'IOC afin de pouvoir utilisé le Codec. De plus, on Crée une paire de fichier sgtl5000.c / sgtl5000.h, ainsi qu'une fonction d'initialisation du codec.
### 3.2 Configuration du CODEC par l’I2C
On commence par récupérer l'adresse du codec sur le bus I2C avec la fonction suivante: 
```
uint16_t getCHIP_ID(uint8_t reg, uint8_t I2C_addr)
{
   uint16_t data = 0;
   HAL_StatusTypeDef status;

   status = HAL_I2C_Mem_Read(&hi2c2, I2C_addr, reg, I2C_MEMADD_SIZE_8BIT, &data, 2, HAL_MAX_DELAY);
   if (status != HAL_OK){
       return -1;
   }
   return data;
}
```
On trouve chipID: 0x3c3c.

Ci-dessous, une exemple de trame I2C (avec le SDA en jaune et le SCL en vert):
![image](https://github.com/user-attachments/assets/f6de6307-a58e-4941-812d-c7960a20d894)

### 3.3 Signaux I2S

Nous allons déclaré La fonction `remplir()` est d'abord appelée pour préparer les données à transmettre. Ensuite, `HAL_SAI_Transmit_DMA` lance la transmission des données depuis le tableau tx_buffer via le périphérique SAI en utilisant le mode DMA. Simultanément, HAL_SAI_Receive_DMA démarre la réception des données dans le tableau rx_buffer en mode DMA. Ces deux opérations permettent de gérer efficacement la transmission et la réception de données audio en parallèle.

![image](https://github.com/user-attachments/assets/3e969c36-447b-4556-9333-4534a2a5d881)

```
void remplir(void) {
    for (int i = 0; i < 128; i++) {
        tx_buffer[i] = 0xA0; // Valeur constante (mi-hauteur pour un signal 16 bits non signé)
    }
}
```

### 3.4 Génération de signal audio

On génère un signal triangulaire dans le ficiher `wave_generator.c`. Ensuite, on envoie le signal sur la sortie audio (LIN_OUT) du codec en faisant une redirection du SAI vers le LIN_OUT (voir fichier `sglt5000.c`  et le callback `HAL_SAI_TxCpltCallback`. Ci-dessous, le signal triangulaire vu à l'oscilloscope.

![image](https://github.com/user-attachments/assets/b42d7984-89a0-4355-b71d-03552f4121cf)





