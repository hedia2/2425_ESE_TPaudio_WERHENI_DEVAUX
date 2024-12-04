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

![image](https://github.com/user-attachments/assets/03030ea0-ef5c-467d-bea4-181019a43e97)


## (b) En mode interruption, :

## (c) Avec un driver sous forme de structure:

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

Nous allons déclaré la fonction MCP23S17_Init() qui permet de :

*renitialisation de pin Reset 

*La commande MCP23S17_Write(0x00, 0x00); configure tous les pins du MCP23S17 comme sorties. Elle écrit la valeur 0x00 dans le registre IODIRA, ce qui définit chaque pin en mode sortie. 

*La commande MCP23S17_Write(0x01, 0x00); écrit la valeur 0x00 dans le registre IODIRB du MCP23S17. Le registre IODIRB contrôle la direction des pins du port B (par opposition au port A contrôlé par le registre IODIRA). En écrivant 0x00, elle configure toutes les pins du port B du MCP23S17 en mode sortie.

aprés Nous allons déclaré la fonction MCP23S17_Write permet d'envoyer une commande d'écriture via SPI pour configurer un registre du MCP23S17. Elle commence par préparer un tableau contenant le code d'écriture, l'adresse du registre et les données à écrire. Ensuite, elle active le signal Chip Select (CS) en le mettant à l'état bas, puis transmet les données via la fonction HAL_SPI_Transmit. 

## 3.2 Configuration du CODEC par l’I2C

![image](https://github.com/user-attachments/assets/f6de6307-a58e-4941-812d-c7960a20d894)

## 3.3 Signaux I2S

![image](https://github.com/user-attachments/assets/3e969c36-447b-4556-9333-4534a2a5d881)





