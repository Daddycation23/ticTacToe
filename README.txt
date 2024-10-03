To compile and run main.c, you must first download raylib. You may download it from https://www.raylib.com/.
Steps to download raylib:
  1. Go to https://www.raylib.com/
  2. Scroll down a little and you will see a tab with "Download Now"
  3. Click on "Download Now"
  4. A window will pop up asking you for donation. You may choose to donate or simply click "No thanks, just take me to the downloads"
  5. Click on "Download". Right beside the download tab should have this description: "raylib 5.0 Windows Installer 64bit (MinGW compiler)"
  6. Choose your desired directory for the download
  7. Wait for the installation to complete
  8. Open the installer once the download is finished
  9. IMPORTANT**: A window will pop up for your to start the installation. Make sure the install path is C:\raylib. This is because the include file path is specified in as C:\\raylib\\raylib\\src\\raylib.h, you are free to makes changes if you know how to include the required libraries but if you want to save the trouble, make sure the file path is C:\raylib
  10. Click install and wait for the installation to complete
  11. Close the window and open VS code or any IDE you prefer to use
  12. Open main.c in from this directory
  13. IMPORTANT**: You need to follow this command exactly and type into the terminal to successfully complie main.c. The command is also available at the bottom of main.c. The command is: gcc -o main main.c -IC:\raylib\w64devkit\x86_64-w64-mingw32\include -LC:\raylib\w64devkit\x86_64-w64-mingw32\lib -lraylib -lopengl32 -lgdi32 -lwinmm
  14. Once successfully compiled, type ./main in the terminal to run the program
