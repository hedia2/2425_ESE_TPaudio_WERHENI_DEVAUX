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


## 3.2 Configuration du CODEC par l’I2C

![image](https://github.com/user-attachments/assets/f6de6307-a58e-4941-812d-c7960a20d894)

## 3.3 Signaux I2S

![image](https://github.com/user-attachments/assets/3e969c36-447b-4556-9333-4534a2a5d881)





