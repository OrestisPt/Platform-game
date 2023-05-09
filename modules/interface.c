#include "interface.h"

Texture starimg;
Texture ballimg;
Sound gameover;
Sound boing;
Sound gotStar;

void interface_init() {
	// Αρχικοποίηση του παραθύρου
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "game");
    SetWindowIcon(LoadImage("assets/ball.png"));
	SetTargetFPS(60);
    InitAudioDevice();
    starimg = LoadTextureFromImage(LoadImage("assets/star.png"));
    ballimg = LoadTextureFromImage(LoadImage("assets/ball.png"));
    gameover = LoadSound("assets/game_over.wav");
    boing = LoadSound("assets/boing.wav");
    gotStar = LoadSound("assets/gotStar.wav");
}

void interface_close() { 
	CloseAudioDevice();
	CloseWindow();
}

void interface_draw_frame(State state) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawTexture(ballimg, SCREEN_WIDTH/3, state_info(state)->ball->rect.y, WHITE);
    List Objects = state_objects(state, SCREEN_WIDTH/3 - state_info(state)->ball->rect.x, state_info(state)->ball->rect.x + 2 * SCREEN_WIDTH/3);
    for(ListNode node = list_first(Objects); node != LIST_EOF; node = list_next(Objects, node)){
        Object obj = list_node_value(Objects, node);
        if(obj->type == PLATFORM)
            if(obj->unstable)
                DrawRectangle(obj->rect.x + (SCREEN_WIDTH/3 - state_info(state)->ball->rect.x), obj->rect.y, obj->rect.width, obj->rect.height,  RED);
            else   
                DrawRectangle(obj->rect.x + (SCREEN_WIDTH/3 - state_info(state)->ball->rect.x), obj->rect.y, obj->rect.width, obj->rect.height,  GREEN);
        else if(obj->type == STAR)
            DrawTexture(starimg, (int)(obj->rect.x + (SCREEN_WIDTH/3 - state_info(state)->ball->rect.x)), (int)obj->rect.y, YELLOW);
    }
    DrawText(TextFormat("%i", state_info(state)->score), 20, 20, 40, GRAY);
    DrawFPS(SCREEN_WIDTH - 80, 0);
    if (!state_info(state)->playing) {
		DrawText(
			"PRESS [ENTER] TO PLAY AGAIN",
			 GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2,
			 GetScreenHeight() / 2 - 50, 20, GRAY
		);
	}
    if(sounds_info(state)->jumping){
        PlaySound(boing);
    }
    if(sounds_info(state)->gotStar){
        PlaySound(gotStar);
    }
    if (state_info(state)->paused && state_info(state)->playing) {
		DrawText(
			"PRESS [P] TO CONTINUE",
			 GetScreenWidth() / 2 - MeasureText("PRESS [P] TO CONTINUE", 20) / 2,
			 GetScreenHeight() / 2 - 50, 20, GRAY
		);
	}
    if (sounds_info(state)->gameover) {
        PlaySound(gameover);
    }
    EndDrawing();
}