import pygame
import random
#initialization
pygame.init()

#creating screen
screen=pygame.display.set_mode((500,500),flags=pygame.FULLSCREEN)

#title and icon
pygame.display.set_caption("Dino Game")
icon = pygame.image.load("dinosaur.png")
pygame.display.set_icon(icon)

#dino
dino_img= pygame.image.load("gamedino.png")
dino_x = 25
dino_iy = 400
dino_y = 400
dino_yvel=0
val=100
#cactus
cactus_img= pygame.image.load("cactus.png")
cactus_x = 500
cactus_y = 400
cactus_speed = 0.2
cactus_px=cactus_x

#gamevar
grav = 0.001
add = 0
jmp = 0
fl = 0
xy = 64 #picture dimensions
a = 0
running = 1
sc = 0
font=pygame.font.SysFont('Arial',20)
#collision box draw (For debugging)[1=true 0=false]
cbox = 1

def collide():
    if dino_x + 1 <= cactus_x + xy - 8 and dino_x + xy - 5 >= cactus_x + 8 and dino_y <= cactus_y + xy and dino_y + xy >= cactus_y:
        return True
    else:
        return False

def cactus_s():
    cactus_speed = random.uniform(0.2,0.5)
    return cactus_speed

def dino(x,y):
    if cbox == 1:
        if collide():
            pygame.draw.rect(screen,(0,0,255),(x + 1, y, xy - 4, xy), 1)
        else:
            pygame.draw.rect(screen,(255,0,0),(x + 1, y, xy - 4, xy), 1)
    screen.blit(dino_img,(x,y))
    

def cactus(x,y):
    if cbox == 1:
        if collide():
            pygame.draw.rect(screen,(0,255,0),(x + 8, y, xy - 16, xy), 1)
        else:
            pygame.draw.rect(screen,(255,0,0),(x + 8, y, xy - 16, xy), 1)
    screen.blit(cactus_img,(x,y))

def reset():
    global dino_yvel, dino_y, cactus_x, cactus_y, cactus_speed, sc
    dino_yvel = 0
    dino_y = dino_iy
    cactus_x = 500
    cactus_y = 400
    cactus_speed = 0.2
    sc = 0

def update():
    global dino_y, dino_iy, dino_yvel, fl, jmp
    if dino_y == dino_iy:
        if jmp == 1 and fl == 0:
            fl = 1
            dino_yvel -= 0.5
    dino_y += dino_yvel
    dino_yvel += grav
    if dino_y > dino_iy:
        fl = 0
        dino_y = dino_iy
        dino_yvel = 0

def score():
    global sc
    if(cactus_x != 500):
        sc += (cactus_px - cactus_x)*0.1
    text=font.render(str(int(sc)),True,(0,0,0))
    screen.blit(text,(50,100))#text.get_width(),text.get_height()))
    
#game loop
def rungame():
    global running
    global cactus_x, cactus_y, dino_x, dino_y, cactus_speed, a, jmp, cactus_px
    while running == 1:

        #RGB Background
        screen.fill((255,255,255))
        score()
        #close button to quit
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = -1
                return
    #check for spacebar
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_SPACE:
                    print("SpaceBar Pressed")
                    jmp = 1
                if event.key == pygame.K_ESCAPE:
                    running=-1
                    break
            if event.type == pygame.KEYUP:
                if event.key == pygame.K_SPACE:
                    print ("Key Released")
                    jmp = 0
        update()
            
        

        cactus_px = cactus_x
        cactus_x  = cactus_x -cactus_speed
        if(cactus_x<-64):
            print()
            cactus_x = 500
            cactus_speed=cactus_s()
            print(cactus_speed)

        #dino position
        dino(dino_x,dino_y)
        cactus(cactus_x,cactus_y)
        
        #update screen
    
        pygame.display.update()
        
        if collide():
            if a == 0:
                print("collide\nPress Enter to restart")
                running = 0   
            a = 1
        else:
            a=0

while 1:
    rungame()
    reset()
    for event in pygame.event.get():
        if event.type == pygame.QUIT or running == -1:
            print("Hello")
            running = -1
            break
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_ESCAPE:
                running=-1
                break
            if event.key == pygame.K_RETURN or event.key == pygame.K_KP_ENTER:
                running = 1
        if event.type == pygame.KEYUP:
            if event.key == pygame.K_SPACE:
                jmp = 0
    if running == -1:
        pygame.quit()
        break
 
