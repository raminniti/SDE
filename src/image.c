#include "image.h"

static SDL_Renderer* renderer;
static struct TileSheet tiles[MAX_TILESHEETS];

void ImageInit(SDL_Renderer* r){
	for (unsigned char i = 0; i < MAX_TILESHEETS; ++i){
		tiles[i].texture = nullptr;
	}
	renderer = r;
}

void ImageLoadTilesheet(const char * filename, unsigned short id, unsigned short w, unsigned short h){

	if (renderer == nullptr){
		printf("Image.c: renderer is null! set with ImageInit(r) \n");
		return;
	}
	if (id > MAX_TILESHEETS - 1){
		printf("Unable to load image %s! load id is higher than MAX_TILESHEETS. \n", filename);
		return;
	}
	if (tiles[id].texture == nullptr){
		SDL_Surface* loaded_surface = SDL_LoadBMP(filename);

		if (loaded_surface == nullptr){
			printf("Unable to load image %s! SDL_image Error: %s\n", filename, SDL_GetError());
			return;
		}
		else{
			// Set Transparent color key
			//Uint32 colorkey = SDL_MapRGB(loaded_surface->format, 0xFF, 0, 0xFF);
			SDL_SetColorKey(loaded_surface, SDL_TRUE, SDL_MapRGB(loaded_surface->format, 0xFF, 0, 0xFF));

			SDL_Texture* new_image = SDL_CreateTextureFromSurface(renderer, loaded_surface);
			SDL_SetTextureScaleMode(new_image, SDL_ScaleModeNearest);

			if (new_image == nullptr)
				printf("Unable to load image %s! SDL_image Error: %s\n", filename, SDL_GetError());

			//store loaded texture properties
			tiles[id].texture = new_image;
			tiles[id].tileWidth = w;
			tiles[id].tileHeight = h;
			tiles[id].width = loaded_surface->w;
			tiles[id].height = loaded_surface->h;
			//images[id].tileAmount = images[id].columns * images[id].rows;

			//clean up
			SDL_FreeSurface(loaded_surface);
		}
	}
}

void ImageDraw(int x, int y, unsigned short img_id){
	SDL_Rect r;

	r.x = x;
	r.y = y;
	r.w = tiles[img_id].width;
	r.h = tiles[img_id].height;
	SDL_RenderCopy(renderer, tiles[img_id].texture, 0, &r);
}

void ImageDrawTile(int x, int y, unsigned short img_id, short sub_id){
	SDL_Rect r;
	SDL_Rect c;

	// create clip of tile area
	r.x = x;
	r.y = y;
	r.w = tiles[img_id].tileWidth;
	r.h = tiles[img_id].tileHeight;
	c.x = sub_id % (tiles[img_id].width / tiles[img_id].tileWidth) * tiles[img_id].tileWidth;
	c.y = (int)(sub_id / (tiles[img_id].width / tiles[img_id].tileWidth)) * tiles[img_id].tileHeight;
	c.w = tiles[img_id].tileWidth;
	c.h = tiles[img_id].tileHeight;
	// draw clip to the screen
	SDL_RenderCopy(renderer, tiles[img_id].texture, &c, &r);
}

//SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
void ImageDrawTileExt(int x, int y, unsigned short img_id, short sub_id, double angle, SDL_Point* center, SDL_RendererFlip flip){
	SDL_Rect r;
	SDL_Rect c;

	// create clip of tile area
	r.x = x;
	r.y = y;
	r.w = tiles[img_id].tileWidth;
	r.h = tiles[img_id].tileHeight;
	c.x = sub_id % (tiles[img_id].width / tiles[img_id].tileWidth) * tiles[img_id].tileWidth;
	c.y = (int)(sub_id / (tiles[img_id].width / tiles[img_id].tileWidth)) * tiles[img_id].tileHeight;
	c.w = tiles[img_id].tileWidth;
	c.h = tiles[img_id].tileHeight;
	// draw clip to the screen
	SDL_RenderCopyEx(renderer, tiles[img_id].texture, &c, &r, angle, center, flip);
}

void ImageDrawText(int x, int y, unsigned short img_id, const char * text){
	char letter = 0;
	int lx = 0;
	int ly = 0;

	for (unsigned short i = 0; i <= SDL_strlen(text); ++i){
		letter = text[i];
		switch (letter){
		case '\n':
			ly += tiles[img_id].tileHeight;
			lx = 0;
			//continue;
			break;
		case ' ':
			lx += tiles[img_id].tileWidth;
			break;
		default:
			//if (letter > 31 && letter < 127)
			ImageDrawTile(x + lx, y + ly, img_id, letter - 32);
			lx += tiles[img_id].tileWidth;
			break;
		}
	}
}

/*
void ImageDrawExt(int x, int y, unsigned short img_id, double angle, SDL_RendererFlip flip) {
    SDL_Rect r = { x, y, tiles[img_id].width, tiles[img_id].height };
    SDL_RenderCopyEx(renderer, tiles[img_id].texture, NULL, &r, angle, NULL, flip);
}
*/

void ImageQuit(){
	//Free up memory from Textures
	for (unsigned char i = 0; i < MAX_TILESHEETS; ++i){
		if (tiles[i].texture != 0){
			SDL_DestroyTexture(tiles[i].texture);
			tiles[i].texture = 0;
		}
	}
}
