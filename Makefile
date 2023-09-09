CC=g++
INC=
LIBS=-lSDL2 -lz
ARGS=-O3
PARGS=
BPATH=build
BNAME=jewels-runtime
TARGET=$(BPATH)/$(BNAME)
DEPS=$(BPATH)/runtime$(EXT) $(BPATH)/main$(EXT) $(BPATH)/grid$(EXT) $(BPATH)/game$(EXT) $(BPATH)/font$(EXT) $(BPATH)/shape$(EXT) $(BPATH)/FrameSet$(EXT) $(BPATH)/Frame$(EXT) $(BPATH)/DotArray$(EXT) $(BPATH)/helper$(EXT) $(BPATH)/PngMagic$(EXT) $(BPATH)/FileWrap$(EXT)
EXT=.o

all: $(TARGET)

$(BPATH)/runtime$(EXT): src/runtime.cpp src/runtime.h
	$(CC) $(ARGS) -c $< $(INC) -o $@

$(BPATH)/main$(EXT): src/main.cpp
	$(CC) $(ARGS) -c $< $(INC) -o $@

$(BPATH)/grid$(EXT): src/grid.cpp src/grid.h
	$(CC) $(ARGS) -c $< $(INC) -o $@

$(BPATH)/game$(EXT): src/game.cpp src/game.h
	$(CC) $(ARGS) -c $< $(INC) -o $@

$(BPATH)/font$(EXT): src/font.cpp src/font.h
	$(CC) $(ARGS) -c $< $(INC) -o $@

$(BPATH)/shape$(EXT): src/shape.cpp src/shape.h
	$(CC) $(ARGS) -c $< $(INC) -o $@

$(BPATH)/FrameSet$(EXT): src/shared/FrameSet.cpp src/shared/FrameSet.h
	$(CC) $(ARGS) -c $< $(INC) -o $@

$(BPATH)/Frame$(EXT): src/shared/Frame.cpp src/shared/Frame.h
	$(CC) $(ARGS) -c $< $(INC) -o $@

$(BPATH)/DotArray$(EXT): src/shared/DotArray.cpp src/shared/DotArray.h
	$(CC) $(ARGS) -c $< $(INC) -o $@

$(BPATH)/helper$(EXT): src/shared/helper.cpp src/shared/helper.h
	$(CC) $(ARGS) -c $< $(INC) -o $@

$(BPATH)/PngMagic$(EXT): src/shared/PngMagic.cpp src/shared/PngMagic.h
	$(CC) $(ARGS) -c $< $(INC) -o $@

$(BPATH)/FileWrap$(EXT): src/shared/FileWrap.cpp src/shared/FileWrap.h
	$(CC) $(ARGS) -c $< $(INC) -o $@

$(TARGET): $(DEPS)
	$(CC) $(ARGS) $(DEPS) $(LIBS) $(PARGS) -o $@

clean:
	rm -f $(BPATH)/*