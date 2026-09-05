import pygame, os, math, sys, random;
import numpy as np
from pygame.locals import *
from pathlib import Path

# Level editor layers
LAYER_TILE = 0
LAYER_OBJECT = 1

# TODO - Move these functions later, maybe make proper wrappers
def RoundToNearest(i, j):
    return i & ~(j-1)

def LoadTilesheet(path, tile_size):
    try:
        sheet = pygame.image.load(path).convert_alpha()
    except pygame.error:
        print(f"Failed to load tilesheet at {path}")
        return []
    sheet_width, sheet_height = sheet.get_size()
    cols = sheet_width // tile_size
    rows = sheet_height // tile_size
    tiles = []
    for y in range(rows):
        for x in range(cols):
            rect = pygame.Rect(x * tile_size, y * tile_size, tile_size, tile_size)
            tiles.append(sheet.subsurface(rect))
    return tiles

def DrawTile(surface, tiles, tile_index, position):
    if 0 <= tile_index < len(tiles):
        surface.blit(tiles[tile_index], position)

class Window(object):
    def __init__(self,title="window",w=1280,h=720):
        pygame.init()
        self.width = w
        self.height = h
        self.scale = 4
        self.title = title
        self.size = (w, h)
        self.window = pygame.display.set_mode(self.size)
        self.surface = pygame.Surface((w/self.scale,h/self.scale))
        self.deltatime = 0.0;
        self.currentLevel = 0;
    def GetSurface(self):
        return self.surface
    def DrawTile(self, tiles, tile_index, position):
        if 0 <= tile_index < len(tiles):
            self.surface.blit(tiles[tile_index], position)
    def Draw(self, img, pos=(0,0)):
        self.window.blit(img, pos)
    def Scale(self):
        pygame.transform.scale(self.surface,self.size,self.window) 
    def Update(self):
        pygame.display.flip()
    def Clear(self):
        self.surface.fill((0,0,0))

class Level:
    def __init__(self):
        self.width = 8
        self.height = 8
        self.size = self.width* self.height 
        #self.currentLevel = 0
        self.chunkId = 0
        self.chunkWidth = 15
        self.chunkHeight = 9
        self.chunkSize = self.chunkWidth * self.chunkHeight
        self.tile_size = 16
        self.tiles = np.full((self.size*self.chunkSize),5, dtype=np.int32)
        self.entities = np.zeros((self.size*self.chunkSize), dtype=np.int32)
        self.chunkTiles = self.tiles[0 : self.chunkSize]
        self.chunkObjects = self.entities[0 : self.chunkSize]
        self.file_path = Path("assets/levels/0.bin")
        self.chunkTilesCopy = self.tiles[0 : self.chunkSize]
        self.chunkObjectsCopy = self.entities[0 : self.chunkSize]
        if self.file_path.is_file():
            print("The file exists.")
            self.Load(self.file_path)
        else:
            print("The file does not exist.")
    def Save(self,path):
        self.file_path = Path(path)
        # Force arrays to 16-bit unsigned integers
        tiles_raw = self.tiles.astype(np.uint16).tobytes()
        entities_raw = self.entities.astype(np.uint16).tobytes()
        with open(path, "wb") as f:
            f.write(tiles_raw)
            f.write(entities_raw)
        f.close()
    def Load(self, path):
        self.file_path = Path(path)
        if self.file_path.is_file():
            # Read the entire flat binary stream of unsigned 16-bit ints
            raw_data = np.fromfile(path, dtype=np.uint16)
            self.tiles = raw_data[0:self.chunkSize*self.size]
            self.entities = raw_data[self.chunkSize*self.size:]#.reshape((9, 15))
            self.SelectChunk(0)
            return True
        else:
            print("The file does not exist.")
            self.ClearLevel()
            return False
        
    def SetId(self, x,y, tile_id):
        if x >= 0 and x < self.chunkWidth and y < self.chunkHeight and y >= 0:
            self.chunkTiles[y*self.chunkWidth+x] = tile_id
    def GetId(self, x,y):
        return self.chunkTiles[y*self.chunkWidth+x]
    def SetObjectId(self, x,y, tile_id):
        self.chunkObjects[y*self.chunkWidth+x] = tile_id
    def GetObjectId(self, x,y):
        return self.chunkObjects[y*self.chunkWidth+x]
    def SelectChunk(self, chunk_id):
        # Store the active chunk ID so the level knows where it is
        self.chunkId = chunk_id
        
        # Calculate the flat memory bounds based on the room index
        start_idx = chunk_id * self.chunkSize
        end_idx = start_idx + self.chunkSize
        
        # Slice the 1D arrays for the active room view
        self.chunkTiles = self.tiles[start_idx:end_idx]
        self.chunkObjects = self.entities[start_idx:end_idx]
    def MoveActiveChunk(self, direction):
        current = self.level.current_level
        world_width = self.level.width
        world_height = self.level.height
        total_rooms = world_width * world_height

        if direction == "left":
            # Make sure we aren't on the left edge of the world grid
            if current % world_width != 0:
                self.level.SelectChunk(current - 1)
        elif direction == "right":
            # Make sure we aren't on the right edge of the world grid
            if (current + 1) % world_width != 0:
                self.level.SelectChunk(current + 1)
        elif direction == "up":
            # Make sure subtracting a row doesn't drop below 0
            if current - world_width >= 0:
                self.level.SelectChunk(current - world_width)
        elif direction == "down":
            # Make sure adding a row doesn't exceed total rooms
            if current + world_width < total_rooms:
                self.level.SelectChunk(current + world_width)

    # Fix later? Draw needs objs from selector to convert the oject id to the proper tile id
    def Draw(self, surface, atlas,objs):
        for y in range(self.chunkHeight):
            for x in range(self.chunkWidth):
                # Calculate 1D index local to this chunk
                index =  (y * self.chunkWidth) + x
                pixel_y = y * self.tile_size
                pixel_x = x * self.tile_size
                tile_id = self.chunkTiles[index]
                if tile_id > 0:
                    # Draw tiles
                    DrawTile(surface, atlas, tile_id, (pixel_x, pixel_y))
                # Draw objects
                obj_id = self.chunkObjects[index]
                if obj_id > 0:
                    DrawTile(surface, atlas, objs[obj_id], (pixel_x, pixel_y))
    def Clear(self):
        #for i in range(self.chunkSize):
        self.chunkTiles.fill(5)
        self.chunkObjects.fill(0)
    def ClearLevel(self):
        self.tiles.fill(5)
        self.entities.fill(0)
    def CopyChunk(self):
        for i in range(self.chunkSize):
            self.chunkTilesCopy[i] = self.chunkTiles[i]
            self.chunkObjectsCopy[i] = self.chunkObjects[i]
    def PasteChunk(self):
        for i in range(self.chunkSize):
            self.chunkTiles[i] = self.chunkTilesCopy[i]
            self.chunkObjects[i] = self.chunkObjectsCopy[i]

class Selector:
    def __init__(self, selector_rect, atlas, tile_size=16, scale=2):
        self.rect = selector_rect
        self.tileSize = tile_size
        self.upscale = scale
        self.atlas = atlas
        self.scaled_tile_size = tile_size * scale# 32x
        self.scrollY = 0
        self.selectedTile=1
        # create data for now, load it in later
        self.objectCatalog={
            0: 0,   #dummy data/nothing
            1: 284, #Player - remove - do not use
            2: 484, #enemy
            3: 285, #soul
            4: 201, #stairs down
            5: 200, #stairs up
            6: 260  #upgrade
        }
    def Scroll(self, offset):
        self.scrollY += offset

    def HandleClick(self, mouse_pos, active_layer):
        # Calculate local column and row relative pos
        cc = int((mouse_pos[0] - self.rect.x)) // self.tileSize
        cr = int((mouse_pos[1] + self.scrollY)) // self.tileSize
        num_tiles = len(self.atlas)
        # Flatten using old formula
        columns = 4
        selected_id = cc + (cr * columns)
        # Safety clamp against maximum available assets
        if active_layer == LAYER_TILE:
            self.selectedTile = selected_id
            return selected_id % num_tiles
        else:
            self.selectedObject = selected_id
            return selected_id % len(self.objectCatalog)

    def Draw(self, surface, atlas, active_layer):
        columns = 4
        rows = 8
        max_visible = columns * rows
        if active_layer == LAYER_TILE:
            for i in range(len(self.atlas)):
                col = i % columns
                row = i // columns
                
                # Calculate screen coordinates factoring in scroll
                x = self.rect.x + (col * self.tileSize)
                y = (row * self.tileSize) - self.scrollY

                if y < 0:
                    continue
                elif y >= self.rect.h:
                    break

                if i < len(atlas):
                    DrawTile(surface, atlas, i, (x, y))
                if self.selectedTile == i:
                    pygame.draw.rect(surface,(255,0,0),(x, y,16, 16),1)
        else:
            for i in range(len(self.objectCatalog)):
                col = i % columns
                row = i // columns
                # Calculate screen coordinates factoring in scroll
                x = self.rect.x + (col * self.tileSize)
                y = (row * self.tileSize) - self.scrollY
                if y < 0:
                    continue
                elif y >= self.rect.h:
                    break
                if i < len(self.objectCatalog):
                    DrawTile(surface, atlas, self.objectCatalog[i], (x, y))
                if self.selectedTile == i:
                    pygame.draw.rect(surface,(255,0,0),(x, y,16, 16),1)

class Tool(object):
    def use(self, grid_x, grid_y, tile_id, room):
        pass

class BrushTool(Tool):
    def use(self, grid_x, grid_y, tile_id, room):
        if 0 <= grid_x < room.width and 0 <= grid_y < room.height:
            room.tiles[grid_y][grid_x] = tile_id

class LevelEditor:
    def __init__(self, window, atlas, level):
        self.window = window
        self.canvas = window.GetSurface()
        self.atlas = atlas
        self.level = level
        #self.tool = BrushTool()
        self.tileSize = 16
        self.selectedTile = 1
        self.selectedObject = 1
        self.activeLayer = LAYER_TILE
        self.rect = pygame.Rect(0, 0, 960/self.window.scale, 576/self.window.scale)
        self.selectorRect = pygame.Rect(992/self.window.scale, 0, 320/self.window.scale, 576/self.window.scale)
        self.minimapRect = pygame.Rect(960/self.window.scale, 400/self.window.scale, 320/self.window.scale, 144/self.window.scale)
        self.consoleRect = pygame.Rect(0, 576/self.window.scale, 960/self.window.scale, 144/self.window.scale)
        self.selector = Selector(self.selectorRect, atlas)
        self.running = True
        self.gridX, self.gridY = 0,0
        self.mouseX, self.mouseY = 0,0
        self.consoleLog = []
        self.brushSize=1
        self.currentLevel = 0
        self.levelDir="assets/levels/"
        self.fileExt =".bin"
    def Log(self, msg):
        self.consoleLog.append(msg)
        if len(self.consoleLog) > 10:
            self.consoleLog.pop(0)
    def HandleEvents(self, events):
        # Loop through the list passed from main
        for event in events:
            if event.type == pygame.QUIT:
                self.running = False
            # Mouse motion
            elif event.type == pygame.MOUSEMOTION:
                x, y = event.pos#m_pos = pygame.mouse.get_pos()
                scale = self.window.scale
                mx = x//scale
                my = y//scale
                self.mouseX, self.mouseY = mx, my
                # Calculate tile coordinates in map
                self.gridX = max(0, min(int(mx // self.tileSize), self.level.chunkWidth - 1))#- mapOffset_x
                self.gridY = max(0, min(int(my // self.tileSize), self.level.chunkHeight - 1))#- mapOffset_y

            # Left mouse button press
            elif event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
                if self.selectorRect.x < self.mouseX:
                    selected_id = self.selector.HandleClick((self.mouseX,self.mouseY), self.activeLayer)
                    
                    if self.activeLayer == LAYER_TILE:
                        self.selectedTile = selected_id
                        self.Log(f"Selected Tile ID: {selected_id}")
                    elif self.activeLayer == LAYER_OBJECT:
                        self.selectedObject = selected_id
                        self.Log(f"Selected Object ID: {selected_id}")
            # Right mouse button
            elif event.type == pygame.MOUSEBUTTONDOWN and event.button == 3:
                if self.mouseX < self.rect.w and self.mouseY < self.rect.h:
                    if self.activeLayer == LAYER_TILE:
                        # Copy tile from level
                        self.selectedTile = self.level.GetId(self.gridX, self.gridY)
                    else:
                        self.selectedObject = self.level.GetObjectId(self.gridX, self.gridY)
                elif self.selectorRect.x < self.mouseX:
                    # Copy tile from selector
                    selected_id = self.selector.HandleClick((self.mouseX,self.mouseY), self.activeLayer)
                    
                    if self.activeLayer == LAYER_TILE:
                        self.selectedTile = selected_id
                        self.Log(f"Selected Tile ID: {selected_id}")
                    elif self.activeLayer == LAYER_OBJECT:
                        self.selectedObject = selected_id
                        self.Log(f"Selected Object ID: {selected_id}")
            #mouse wheel to adjust brush size and tile selection
            elif event.type == pygame.MOUSEWHEEL:
                if self.mouseX < self.rect.w and self.mouseY < self.rect.h:
                    #change brush size if cursor is over map
                    if event.y > 0:
                        if self.brushSize < 6:
                            self.brushSize+=1
                    elif event.y < 0:
                        if self.brushSize > 1:
                            self.brushSize-=1
                else:
                    if event.y > 0:
                        self.selector.Scroll(-128)
                    elif event.y < 0:
                        self.selector.Scroll(128)
            # Key pressese
            elif event.type == pygame.KEYDOWN:
                current = self.level.chunkId
                world_width = self.level.width # 8 rooms wide
                # Change chunk/room
                if event.key == pygame.K_LEFT:
                    if current % world_width != 0:
                        current -= 1
                        self.level.SelectChunk(current)
                        self.Log(f"Current room ({current % self.level.width},{current // self.level.width})")
                elif event.key == pygame.K_RIGHT:
                    if (current + 1) % world_width != 0:
                        current += 1
                        self.level.SelectChunk(current)
                        self.Log(f"Current room ({current % self.level.width},{current // self.level.width})")
                elif event.key == pygame.K_UP:
                    if current - world_width >= 0:
                        current -= world_width
                        self.level.SelectChunk(current)
                        self.Log(f"Current room ({current % self.level.width},{current // self.level.width})")
                elif event.key == pygame.K_DOWN:
                    if current + world_width < self.level.size:
                        current += world_width
                        self.level.SelectChunk(current)
                        self.Log(f"Current room ({current % self.level.width},{current // self.level.width})")
                # Toggle mode tiles/ objects
                elif event.key == pygame.K_TAB:
                    self.selector.scrollY = 0
                    if self.activeLayer == LAYER_TILE:
                        self.activeLayer = LAYER_OBJECT
                    else:
                        self.activeLayer = LAYER_TILE
                
                elif event.key == pygame.K_s:
                    path = self.levelDir+str(self.currentLevel)+self.fileExt
                    self.level.Save(path)
                    self.Log("Saved as- "+str(path))
                elif event.key == pygame.K_l:
                    path = self.levelDir+str(self.currentLevel)+self.fileExt
                    if self.level.Load(path):
                        self.Log("Loaded file- "+ path)
                elif event.key == pygame.K_r:
                    #replace all
                    tid = self.level.GetId(self.gridX, self.gridY)
                    for r in range(self.level.chunkHeight):
                        for c in range(self.level.chunkWidth):
                            if(self.level.GetId(c, r)==tid):
                                self.level.SetId(c, r, self.selectedTile)
                elif event.key == pygame.K_z:
                    self.level.Clear()

                elif event.key == pygame.K_c:
                    self.level.CopyChunk()
                elif event.key == pygame.K_v:
                    self.level.PasteChunk()
                elif event.key == pygame.K_PAGEUP:
                    if self.currentLevel > 0:
                        self.currentLevel -= 1
                        path = self.levelDir+str(self.currentLevel)+self.fileExt
                        if self.level.Load(path):
                            self.Log("Loaded file- "+ path)
                        else:
                            self.Log("New file "+ path)
                elif event.key == pygame.K_PAGEDOWN:
                    self.currentLevel += 1
                    path = self.levelDir+str(self.currentLevel)+self.fileExt
                    if self.level.Load(path):
                        self.Log("Loaded file- "+ path)
                    else:
                        self.Log("New file- "+ path)
                        
    def Update(self):
        # Continuous inputs go here
        mouse_buttons = pygame.mouse.get_pressed()
        # Left button held
        if mouse_buttons[0]:  
            # Confirm mouse is inside the map area
            if self.mouseX < self.rect.w and self.mouseY < self.rect.h:
                if self.activeLayer == LAYER_TILE:
                    self.level.SetId(self.gridX, self.gridY, self.selectedTile)
                    for y in range(self.brushSize):
                        for x in range(self.brushSize):
                            c = self.gridX + x
                            r = self.gridY + y
                            self.level.SetId(c, r, self.selectedTile)
                else:
                    self.level.SetObjectId(self.gridX, self.gridY, self.selectedObject)
        if mouse_buttons[1]:
            pass
    def Draw(self):
        self.level.Draw(self.canvas, self.atlas, self.selector.objectCatalog)
        self.selector.Draw(self.canvas, self.atlas, self.activeLayer)
        pygame.draw.rect(self.canvas,(255,0,0),(self.gridX*self.tileSize, self.gridY*self.tileSize, self.brushSize*self.tileSize, self.brushSize*self.tileSize),1)
        self.window.Scale()
        for i in range(len(self.consoleLog)):
            self.window.Draw(font.render(self.consoleLog[i],True,(255,255,255)),(8,592+(i*12)))
        self.window.Update()
    def Quit(self):
        pygame.display.quit()
        pygame.quit()
        sys.exit()

if __name__ == "__main__":
    window = Window()
    atlas = LoadTilesheet("assets/images/atlas.bmp", 16)
    canvas = window.GetSurface()
    lvl = Level()
    lvled = LevelEditor(window, atlas, lvl)
    clock = pygame.time.Clock()
    font = pygame.font.SysFont("Arial", 12)#font = pygame.font.Font(os.path.join(os.getcwd(),'FreeMonoBold.ttf'),12)
    random.seed(0)
    #add intro message
    lvled.Log("Check README.md for controls")

    while lvled.running:
        window.Clear()
        events = pygame.event.get()

        lvled.HandleEvents(events)
        lvled.Update()
        lvled.Draw()
        clock.tick(60)
        
    lvled.Quit()


'''
Todo:
add more tools
add lvl editor type objects (player spawn, doors, ladders, etc)

alt for numpy
#import array; grid = array.array('i', [0] * (width * height))

old numpy save/load funcs
    def Save(self, path):
        # Concatenate arrays into one continuous memory block
        combined_data = np.concatenate((self.tiles, self.entities))
        # Write the raw int32 bytes
        combined_data.tofile(path)

    def Load(self, path):
        data = np.fromfile(path, dtype=np.int32)
        half = len(data)//2
        self.tiles = data[:half]
        self.object = data[half:]
        self.chunkTiles = self.tiles[0 : self.chunkSize]
        self.chunkObjects = self.entities[0 : self.chunkSize]


can use something like this to track and not show objects in the tile selection capping it
        ATLAS_CONFIG = {
    "world":   {"start_row": 0,  "tile_size": 8},
    "objects": {"start_row": 8,  "tile_size": 16},
}

def IsPointInRect(point, rect):
    """
    Checks if a point (x, y) is inside a rectangle.
    rect format: (min_x, min_y, max_x, max_y)
    """
    x, y = point
    min_x, min_y, max_x, max_y = rect
    return min_x <= x <= max_x and min_y <= y <= max_y # Change '<=' to '<' to exclude the borders

'''