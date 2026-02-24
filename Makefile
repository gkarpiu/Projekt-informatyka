# -------- Compiler --------
CXX := g++
CC  := gcc

CXXFLAGS := -Iinclude -MMD -MP
CFLAGS   := -Iinclude -MMD -MP

# NOTE: -lgl is OUR static lib (libgl.a), not OpenGL
LDFLAGS := -Llib -lgl -lglfw3 -lopengl32 -lgdi32 -lws2_32

# -------- Directories --------
OBJDIR := objects
LIBDIR := lib

# -------- Shared sources --------
COMMON_CPP := Camera.cpp Engine.cpp Physics.cpp
COMMON_OBJ := $(patsubst %.cpp,$(OBJDIR)/%.o,$(COMMON_CPP))

# -------- Targets --------
all: libgl game

# -------- Static library (GLAD) --------
libgl: $(LIBDIR)/libgl.a

$(LIBDIR)/libgl.a: $(OBJDIR)/gl.o | $(LIBDIR)
	ar rcs $@ $^

# -------- Executables --------
game: $(OBJDIR)/main.o $(COMMON_OBJ) $(LIBDIR)/libgl.a
	$(CXX) $^ -o game $(LDFLAGS)

# -------- Object rules --------
$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/gl.o: src/gl.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# -------- Directories --------
$(OBJDIR):
	mkdir -p $(OBJDIR)

$(LIBDIR):
	mkdir -p $(LIBDIR)

# -------- Dependencies --------
-include $(OBJDIR)/*.d

# -------- Clean --------
clean:
	rm -f $(OBJDIR)/*.o $(OBJDIR)/*.d
	rm -f $(LIBDIR)/libgl.a
	rm -f game
