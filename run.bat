@del /Q build\tbdriller.exe
@CL /Os /Iinclude main.cpp /link opengl32.lib lib\sfml-window.lib lib\sfml-system.lib lib\sfml-network.lib lib\sfml-graphics.lib lib\sfml-audio.lib lib\openal32.lib lib\flac.lib lib\freetype.lib lib\ogg.lib lib\vorbis.lib lib\vorbisfile.lib lib\vorbisenc.lib /out:build/tbdriller.exe
@del main.obj
@del /Q build\images
@del /Q build\sfx
@xcopy images build\images /i /E
@xcopy sfx build\sfx /i /E
@cd build/
@tbdriller.exe
@cd ..