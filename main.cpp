#include "FEHLCD.h"
#include "FEHRandom.h"
#include <string.h>
#include <vector>
#include "myLib"
#include <thread>
#include <chrono>
#include <time.h>
using namespace std;

// Adds features to all the static screens
void Initialize() {
    LCD.SetFontColor(LCD.White);
    LCD.SetDrawColor(LCD.Scarlet);

    // Add buttons to the main menu
    mainMenu.addButton(new InstructionsButton);
    mainMenu.addButton(new StartGameButton);
    mainMenu.addButton(new ViewStats);
    mainMenu.addButton(new Quit);
    mainMenu.addButton(new CreditsButton);
    mainMenu.addButton(&easy);
    mainMenu.addButton(&med);
    mainMenu.addButton(&hard);
    mainMenu.Draw();

    // Add the text necessary for the instructions
    instructions.addButton(new MainMenuButton);
    instructions.addTextBox(new TextBox("You control the [COLOR] circle", 0, 0));
    instructions.addTextBox(new TextBox("Don't hit the [COLOR] squares", 0, 20));
    instructions.addTextBox(new TextBox("Grab the small [COLOR] square to score", 0, 40));
    instructions.addTextBox(new TextBox("Collect [NUMBER] to win", 0, 60));
    instructions.addTextBox(new TextBox("Powerups will appear on occasion", 0, 80));
    instructions.addTextBox(new TextBox("Don't run out of time", 0, 100)); 

    // Give the statistics screen a main menu button
    statsScreen.addButton(new MainMenuButton);

    // Add the text for the credits screen
    credits.addButton(new MainMenuButton);
    credits.addTextBox(new TextBox("Developed By:", 0, 0));
    credits.addTextBox(new TextBox("Todd Byrd", 0, 20));
    credits.addTextBox(new TextBox("Megan Nicholson", 0, 40));
    credits.addTextBox(new TextBox("Core Ireland", 0, 60));

    // Add the powerups and buttons to the game screen
    gameScreen.addEntity(&invisiblePU);
    gameScreen.addEntity(&slowPU);
    gameScreen.addEntity(&timePU);
    gameScreen.addEntity(&lifePU);
    gameScreen.addButton(new UpButton);
    gameScreen.addButton(new DownButton);
    gameScreen.addButton(new RightButton);
    gameScreen.addButton(new LeftButton);

    // Add text and main menu button to the ending screen
    endScreen.addTextBox(new TextBox("GAME OVER", 100, 100));
    endScreen.addButton(new MainMenuButton);

    currScreen = mainMenu;
}

int main() {
    Initialize();

    // Loop until the player hooses to exit
    while (!quitting) {

        // Get the touched coordinates
        int touchX, touchY;
        while (!playing && !LCD.Touch(&touchX, &touchY));
        if (LCD.Touch(&touchX, &touchY) || !playing) {
            currScreen.checkPressed(touchX, touchY); 
        }
        
        // Draw whatever screen is currently being displayed
        currScreen.Draw();
        currScreen.moveEntities();

        // Do the following if the game is actively being played
        if (playing) {
            gameScreen.CheckHits();

            spawnNewPU();

            // Write the time left, score, and lives
            LCD.WriteAt(currStat.score, 52, 0);
            LCD.WriteAt(currStat.life, 52, 15);
            LCD.SetDrawColor(LCD.Black);
            LCD.FillRectangle(52, 30, 30, 15);
            LCD.WriteAt((int) currStat.time, 52, 30);
            currStat.time--;

            // Display whether the enemies are slowed or the player is invincible
            if (invincibility.applied) {
                LCD.FillRectangle(0, 45, 72, 15);
                LCD.WriteAt("Invincible", 0, 45);
            }
            if (slowEnemy.applied) {
                LCD.FillRectangle(0, 60, 93, 15);
                LCD.WriteAt("Slow Enemy", 0, 60);
            }

            // Tick timers on status effects such as invincibility
            for (int i = 0; i < 2; i++) {
                if ((*allEffects[i]).timeLeft > 0) {
                    (*allEffects[i]).timeLeft -= 1;
                }

                if ((*allEffects[i]).timeLeft == 0) {
                    (*allEffects[i]).applied = false;

                    LCD.FillRectangle(0, 45, 72, 15);
                    LCD.FillRectangle(0, 60, 93, 15);
                    (*allEffects[i]).timeLeft = -1;
                }
            }

            // If the game is over, switch the screen and display the appropriate message
            if (gameOver()) {
                playing = false;
                SaveStats();
                if (win) {
                    endScreen.addTextBox(new TextBox("YOU WIN", 100, 120));
                } else {
                    endScreen.addTextBox(new TextBox("YOU LOSE", 100, 120));
                }
                currScreen = endScreen;
                currScreen.Draw();
                mainMenu.addTextBox(new TextBox("Press Quit and restart to play again", 0, 150));
            }
        }
    }
    
    LCD.Clear(LCD.Black);
    return 0;
}
