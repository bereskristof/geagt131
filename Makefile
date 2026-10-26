windows: libobj/libobj.a
	gcc -Ilibobj/include -Isdl_common -Llibobj sdl_common/load_tex.c sdl_common/common_math.c game_scene/tile/tile.c game_manager/game_manager.c game_scene/game_scene.c game_scene/camera/camera.c game_scene/play_piece/play_piece.c game_scene/dice/dice.c -lobj -lmingw32 -lSDL2main -lSDL2_image -lSDL2 -lopengl32 ./main.c -o main.exe -Wall -Wextra -Wpedantic

windows-debug: libobj/libobj.a
	gcc -Ilibobj/include -Isdl_common -Llibobj sdl_common/load_tex.c sdl_common/common_math.c game_scene/tile/tile.c game_manager/game_manager.c game_scene/game_scene.c game_scene/camera/camera.c game_scene/play_piece/play_piece.c game_scene/dice/dice.c -lobj -lmingw32 -lSDL2main -lSDL2_image -lSDL2 -lopengl32 ./main.c -o main.exe -Wall -Wextra -Wpedantic -Wno-unused-parameter -DEBUG

libobj/libobj.a:
	cd ./libobj/ && make && cd ..
