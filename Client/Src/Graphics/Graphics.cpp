#include "../Window.hpp"
#include "../Graphics.hpp"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include <string>
#include <string.h>
#include <FTGL/ftgl.h>



int LoadPNG(const char *filename, GLuint *Texture)
{
        FILE         *infile;         /* PNG file pointer */
        png_structp   png_ptr;        /* internally used by libpng */
        png_infop     info_ptr;       /* user requested transforms */
        
        char         *image_data;      /* raw png image data */
        char         sig[8];           /* PNG signature array */
        /*char         **row_pointers;   */
        
        int           bit_depth;
        int           color_type;
        
        unsigned long width;            /* PNG image width in pixels */
        unsigned long height;           /* PNG image height in pixels */
        unsigned int rowbytes;         /* raw bytes at row n in image */
        
        image_data = NULL;
        unsigned int i;
        png_bytepp row_pointers = NULL;
        
        /* Open the file. */
        infile = fopen(filename, "rb");
        if (!infile) {
                return 0;
        }
        
        
        /*
         *              13.3 readpng_init()
         */
        
        /* Check for the 8-byte signature */
        fread(sig, 1, 8, infile);
        
        if (!png_check_sig((unsigned char *) sig, 8)) {
                fclose(infile);
                return 0;
        }
        
        /* 
         * Set up the PNG structs 
         */
        png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (!png_ptr) {
                fclose(infile);
                return 0;    /* out of memory */
        }
        
        info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr) {
                png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
                fclose(infile);
                return 0;    /* out of memory */
        }
        
        
        /*
         * block to handle libpng errors, 
         * then check whether the PNG file had a bKGD chunk
         */
        if (setjmp(png_jmpbuf(png_ptr))) {
                png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
                fclose(infile);
                return 0;
        }
        
        /* 
         * takes our file stream pointer (infile) and 
         * stores it in the png_ptr struct for later use.
         */
        /* png_ptr->io_ptr = (png_voidp)infile;*/
        png_init_io(png_ptr, infile);
        
        /*
         * lets libpng know that we already checked the 8 
         * signature bytes, so it should not expect to find 
         * them at the current file pointer location
         */
        png_set_sig_bytes(png_ptr, 8);
        
        /* Read the image */
        
        /*
         * reads and processes not only the PNG file's IHDR chunk 
         * but also any other chunks up to the first IDAT 
         * (i.e., everything before the image data).
         */
        
        /* read all the info up to the image data  */
        png_read_info(png_ptr, info_ptr);
        
        png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, 
                                 &color_type, NULL, NULL, NULL);
        
        
        /* Set up some transforms. */
        if (color_type & PNG_COLOR_MASK_ALPHA) {
                png_set_strip_alpha(png_ptr);
        }
        if (bit_depth > 8) {
                png_set_strip_16(png_ptr);
        }
        if (color_type == PNG_COLOR_TYPE_GRAY ||
                color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
                png_set_gray_to_rgb(png_ptr);
        }
        if (color_type == PNG_COLOR_TYPE_PALETTE) {
                png_set_palette_to_rgb(png_ptr);
        }
        
        /* Update the png info struct.*/
        png_read_update_info(png_ptr, info_ptr);
        
        /* Rowsize in bytes. */
        rowbytes = png_get_rowbytes(png_ptr, info_ptr);
        
        
        /* Allocate the image_data buffer. */
        if ((image_data = (char *) malloc(rowbytes * height))==NULL) {
                png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
                return 0;
    }
        
        if ((row_pointers = (png_bytepp)malloc(height*sizeof(png_bytep))) == NULL) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        free(image_data);
        image_data = NULL;
        return 0;
    }
        
        
    /* set the individual row_pointers to point at the correct offsets */
        
    for (i = 0;  i < height;  ++i)
        row_pointers[height - 1 - i] = (png_byte*)(image_data + i*rowbytes);
        
        
    /* now we can go ahead and just read the whole image */
    png_read_image(png_ptr, row_pointers);
        
    /* and we're done!  (png_read_end() can be omitted if no processing of
     * post-IDAT text/time/etc. is desired) */
        
        /* Clean up. */
        free(row_pointers);
        
        /* Clean up. */
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(infile);
        
        glGenTextures(1, Texture);
        
        printf("(loadTexture) width: %ld height: %ld\n", width, height); 
        
        /* create a new texture object
         * and bind it to texname (unsigned integer > 0)
         */
        glBindTexture(GL_TEXTURE_2D, *Texture);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, 
                                 GL_RGB, GL_UNSIGNED_BYTE, image_data);
        return 1;
                
}

namespace Graphics
{
	GLuint _Terrain;
	FTFont *_Font[2];
	GLuint LoadTexture(std::string filename)
	{
		//Simple Load texture
		GLuint TempTex;
		if(!LoadPNG(filename.c_str(), &TempTex))
			printf("Couldn't Open %s\n", filename.c_str());
		return TempTex;
	}
	void DrawRect(sfRect Quad, GLuint Tex)
	{
		// we need to NOT use immediate mode, is super slow compared to other ways
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, Tex);
		glBegin(GL_QUADS);
		static float colour = 0.0f;
		colour += 0.01f;
		if(colour > 1.0f)
			colour = 0.0f;
			glColor4f(colour, 1.0, 1.0, 1.0);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(Quad.x, Quad.y, Quad.z); // Top Left
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(Quad.x + Quad.w, Quad.y, Quad.z); // Top Right
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(Quad.x + Quad.w, Quad.y + Quad.h, Quad.z); // Bottom Right
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(Quad.x, Quad.y + Quad.h, Quad.z); // Bottom Left
		glEnd();
		glPopMatrix();
	}
	
	void DrawCube(sfCube Cube, float *TexCoords, float *ColourCoords)
	{
		// we need to NOT use immediate mode, is super slow compared to other ways

		GLfloat vertices[] = {
				// Top Quad
				Cube.x + Cube.w, Cube.y, -32 + Cube.z, // Top Right
				Cube.x, Cube.y, -32 + Cube.z, // Top Left
				Cube.x, Cube.y + Cube.h, -32 + Cube.z, // Bottom Left 
				Cube.x + Cube.w, Cube.y + Cube.h, -32 + Cube.z, // Bottom Right
				// Bottom
				Cube.x + Cube.w, Cube.y, -32 + Cube.z + Cube.d, // Top Right
				Cube.x, Cube.y, -32 + Cube.z + Cube.d, // Top Left
				Cube.x, Cube.y + Cube.h, -32 + Cube.z + Cube.d, // Bottom Left 
				Cube.x + Cube.w, Cube.y + Cube.h, -32 + Cube.z + Cube.d, // Bottom Right
				// Front
				Cube.x + Cube.w, Cube.y, -32 + Cube.z, // Top Right
				Cube.x, Cube.y, -32 + Cube.z, // Top Left
				Cube.x, Cube.y, -32 + Cube.z + Cube.d, // Bottom Left 
				Cube.x + Cube.w, Cube.y, -32 + Cube.z + Cube.d, // Bottom Right
				// Left
				Cube.x, Cube.y, -32 + Cube.z, // Top Right
				Cube.x, Cube.y + Cube.w, -32 + Cube.z, // Top Left
				Cube.x, Cube.y + Cube.w, -32 + Cube.z + Cube.d, // Bottom Left 
				Cube.x, Cube.y, -32 + Cube.z + Cube.d, // Bottom Right
				// Right
				Cube.x + Cube.w, Cube.y, -32 + Cube.z, // Top Right
				Cube.x + Cube.w, Cube.y + Cube.w, -32 + Cube.z, // Top Left
				Cube.x + Cube.w, Cube.y + Cube.w, -32 + Cube.z + Cube.d, // Bottom Left 
				Cube.x + Cube.w, Cube.y, -32 + Cube.z + Cube.d, // Bottom Right
				// Back
				Cube.x + Cube.w, Cube.y + Cube.w, -32 + Cube.z, // Top Right
				Cube.x, Cube.y + Cube.w, -32 + Cube.z, // Top Left
				Cube.x, Cube.y + Cube.w, -32 + Cube.z + Cube.d, // Bottom Left 
				Cube.x + Cube.w, Cube.y + Cube.w, -32 + Cube.z + Cube.d, // Bottom Right
				
				}; // 24 of vertex coords
		// activate and specify pointer to vertex array
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		//glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _Terrain);
		glClientActiveTexture(GL_TEXTURE0);
		glTexCoordPointer(2, GL_FLOAT, 0, TexCoords);
		glColorPointer(4, GL_FLOAT, 0, ColourCoords);
		glVertexPointer(3, GL_FLOAT, 0, vertices);

		// draw a cube
		glDrawArrays(GL_QUADS, 0, 24);

		// deactivate vertex arrays after drawing
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
	void DrawLines(sCoord *Lines, int Count)
	{
		// we need to NOT use immediate mode, is super slow compared to other ways
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, 0);
		glBegin(GL_LINE_STRIP);
		static float colour = 0.0f;
		colour += 0.01f;
		if(colour > 1.0f)
			colour = 0.0f;
			glColor4f(colour, 1.0, 1.0, 1.0);
			for(int a = 0; a < Count; ++a)
				glVertex3f(Lines[a].X, Lines[a].Y, Lines[a].Z);
		glEnd();
		glPopMatrix();
	}
	// TODO: Make this complete
	void DrawPlayer(cPlayer *Player, cPlayer* Relation)
	{
		//Get our bounding box for the name
		FTBBox Test = _Font[0]->BBox((char*)Player->GetName(), -1, FTPoint(0, 0, 0));
		// Get the difference between the two sides;
		FTPoint Test2 = Test.Upper() - Test.Lower();
		const f32 ScaleX = 0.6, ScaleY = 0.6;
		glPushMatrix();
			// Make sure there is no texture
			glBindTexture(GL_TEXTURE_2D, 0);
			// Set it to white for the text
			glColor4f(1.0, 1.0, 1.0, 1.0);
			// Translate to the location
			glTranslatef(Player->Coord().X - Relation->Coord().X - Test2.X() / 2 * ScaleX, Player->Coord().Y - Relation->Coord().Y + 2, -32 + Player->Coord().Z);
			// Scale the font down in size
			glScalef(ScaleX, ScaleY, 1);
			// Now render it
			// TODO: Put a grey rectangle underneath it to make it pop more
			_Font[0]->Render((char*)Player->GetName(), -1, FTPoint(0, 0, 0));
		glPopMatrix();
		// TODO: Z relation
		// TODO: Show an actual player model
		float TexCoords[48], ColourCoords[96];
		DrawCube(	{Player->Coord().X - Relation->Coord().X, Player->Coord().Y - Relation->Coord().Y, Player->Coord().Z, 0.5, 0.5, 2.0},
					GetTexCoord(TILE_TYPE::GRASS, TexCoords),
					GetColourCoord(TILE_TYPE::GRASS, ColourCoords) );
	}

	void DrawTile(cTile *Tile, cPlayer *Relation)
	{
		// TODO: Adjust Z here later
		float TexCoords[48], ColourCoords[96];
		Graphics::DrawCube({Tile->_X - Relation->Coord().X, Tile->_Y - Relation->Coord().Y, Tile->_Z, 1, 1, 1}, GetTexCoord(Tile->Type(), TexCoords), GetColourCoord(Tile->Type(), ColourCoords));
	}
	void DrawText(const char *Text, sCoord Coord, bool TwoD)
	{
		_Font[TwoD]->Render(Text, -1, FTPoint(Coord.X, Coord.Y, Coord.Z));
	}
	void Init()
	{
		if (glewInit() != GLEW_OK)
		{
			printf("glewInit() failed! Does your video card support OpenGL 2.x?\n");
			return;	// TODO: fail
		}
		sRect Dims = Windows::GetScreenDim();
		glViewport(Dims.x, Dims.y, Dims.w, Dims.h); // Set our viewport to the window size
		glMatrixMode(GL_PROJECTION);                        // Select The Projection Matrix
		glLoadIdentity();                           // Reset The Projection Matrix
		gluPerspective(45.0f, float(Dims.w) / float(Dims.h), 0.1f, 128.0f);
		glMatrixMode(GL_MODELVIEW);                        // Select The Projection Matrix
		glLoadIdentity();                           // Reset The Projection Matrix
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);                   // Black Background
		glRotatef(-25, 1, 0,0);
		glTranslated(0, 16, 0);

		glEnable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthFunc(GL_LEQUAL);  // Type Of Depth Testing
		glEnable(GL_DEPTH_TEST); // Enable Depth Testing
		glAlphaFunc(GL_NOTEQUAL, 0.0);

		glClearDepth(1.0f);                         // Depth Buffer Setup
		glEnable(GL_DEPTH_TEST);                        // Enables Depth Testing

		_Terrain = Graphics::LoadTexture("Resources/terrain.png");

		_Font[0] = new FTPolygonFont("/usr/share/fonts/truetype/freefont/FreeSerif.ttf");
		_Font[1] = new FTPixmapFont("/usr/share/fonts/truetype/freefont/FreeSerif.ttf");
		if(_Font[0]->Error() || _Font[1]->Error())
		{
			printf("Couldn't load font file!\n");
			return;
		}
		_Font[0]->FaceSize(1);
		_Font[1]->FaceSize(16);
	}
	void MoveCamera(bool Down)
	{
		static f32 Step = 0.0f;
		// This makes sure there is 25 steps
		// Going from -25 to 0
		if(Down)
		{
			if(Step > -25)
				Step -= 1.0f;
		}
		else
		{
			if(Step < 0)
				Step += 1.0f;
		}
		glLoadIdentity();                           // Reset The Projection Matrix
		// So the closest we want to get
		// is a rotation of -60 on the X axis
		// and translating to (0, 10, 25)
		glRotatef(-25 + Step * 1.4f, 1, 0, 0);
		glTranslatef(0, 16 + Step * 0.24f, -Step);
	}
	void Shutdown()
	{
		delete _Font[0];
		delete _Font[1];
	}
	void Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	}
	void Swap()
	{
		// Just a redirect
		Windows::SwapBuffers();
	}
}
