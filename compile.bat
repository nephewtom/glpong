cl.exe /c /D_CRT_SECURE_NO_WARNINGS /W3 /nologo /Iexternal /Iexternal\imgui /Isrc\util /Zi /MDd /EHsc /Fdbin\pong.pdb /Fo:obj/ src\*.cpp

:: These ones compile imgui, glad and util
:: cl.exe /c /W3 /nologo /Iexternal /Iexternal\imgui /Zi /MDd /EHsc /Fdbin\imgui.pdb /Fo:obj/imgui/ external\imgui\*.cpp
:: cl.exe /c /W3 /nologo /Iexternal /Zi /MDd /EHsc /Fdbin\glad.pdb /Fo:obj/util/ src\util\glad.c
:: cl.exe /c /W3 /nologo /Iexternal /Isrc\util /Zi /MDd /EHsc /Fdbin\util.pdb /Fo:obj/util/ src\util\*.cpp