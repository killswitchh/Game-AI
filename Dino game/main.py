import pygame
import random
#initialization
pygame.init()

#creating screen
screen=pygame.display.set_mode((500,500))

#title and icon
pygame.display.set_caption("Dino Game")
icon = pygame.image.load("dinosaur.png")
pygame.display.set_icon(icon)

#dino
dino_img= pygame.image.load("gamedino.png")
dino_x = 25
dino_y = 400
val=100
#cactus
cactus_img= pygame.image.load("cactus.png")
cactus_x = 500
cactus_y = 400
cactus_speed = 0.1

def cactus_s():
    cactus_speed = random.uniform(0.1,0.5)
    return cactus_speed

def dino(x,y):
    screen.blit(dino_img,(x,y))

def cactus(x,y):
    screen.blit(cactus_img,(x,y))
add=0
#game loop
running = True
while running:
    #RGB Background
    screen.fill((220,210,200))
    #close button to quit
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        
    #check for spacebar
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_SPACE:
                print("SpaceBar Pressed")
                add+=val
                dino_y -= val 
        if event.type == pygame.KEYUP:
            if event.key == pygame.K_SPACE:
                print ("Key Released")
                dino_y += add
                add=0
    #dino position
    dino(dino_x,dino_y)

    cactus_x  = cactus_x -cactus_speed
    if(cactus_x<-64):
        print()
        cactus_x = 500
        cactus_speed=cactus_s()
        print(cactus_speed)

    cactus(cactus_x,cactus_y)
    #update screen
    
    pygame.display.update()

