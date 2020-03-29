import pygame
import random
import threading
import time

mutation_r = 0.001 #between 0, 1

def relu(a):
    if a>=0:
        return a
    else:
        return 0

def binac(a):
    if a >= 0:
        return 1
    else:
        return 0
    
class layer:
    def __init__(self, p = 0, c = 0, lis = [], af = relu):
        self.pl = p
        self.cl = c
        self.w = lis
        self.op = [0 for i in range(self.cl)]
        self.af = af
    def newlayer(self, p, c, af = relu):
        self.w = [[random.uniform(-1, 1) for i in range(p)] for i in range(c)]
        self.pl = p
        self.cl = c
        self.op = [0 for i in range(self.cl)]
        self.af = af
    def calcop(self, inp):
        for i in range(self.cl):
            self.op[i] = 0
        n = 0
        for i in self.w:
            c = 0
            for j in range(self.pl):
                c += inp[j]*i[j]
            self.op[n] = self.af(c)
            n += 1
    def displayer(self):
        for i in range(self.cl):
            print("w for neuron ",i,": ",self.w[i])
        for i in range(self.cl):
            print("OP for neuron ",i,": ",self.op[i])

class network:
    def __init__(self, nl = 0, lays = 0):
        self.nl = nl
        self.layers = lays
        self.numlayers = len(lays)
        if lays != 0:
            no = 0
            for i in lays:
                no += i.cl
            self.numneur = no
        else:
            self.numneur = 0
        self.op = []
        self.score = 0
        self.sprob = 0
    def newnetwork(self, nl, lays):
        self.nl = nl
        self.layers = lays
        self.op = [0 for i in range(lays[len(lays) - 1].cl)]
        for i in lays:
            no += i.cl
        self.numneur = no
        self.score = 0
        self.sprob = 0
    def runnetwork(self, inp):
        for lay in self.layers:
            lay.calcop(inp)
            inp = lay.op
        self.op = lay.op
    def dispnw(self):
        print("----------------------------------------------------------")
        for i in self.layers:
            i.displayer()
        print("----------------------------------------------------------\n\n")
    def dispop(self):
        print("----------------------------------------------------------")
        print("Result: ", self.op)
        print("----------------------------------------------------------\n\n")

#----------------------------------------------------------------------------- game stuff
        
class hitbox:
    def __init__(self, x, y, w, h):
        self.x = x
        self.y = y
        self.w = w
        self.h = h
    def update(self, x, y, w, h):
        self.x = x
        self.y = y
        self.w = w
        self.h = h
    def draw(self, screen, col):
        if col == 1:
            pygame.draw.rect(screen, (0, 255, 0), (self.x, self.y, self.w, self.h), 1)
        else:
            pygame.draw.rect(screen, (255, 0, 0), (self.x, self.y, self.w, self.h), 1)
    def collide(self, hb):
        if(self.x <= hb.x + hb.w and self.x + self.w >= hb.x and self.y <= hb.y + hb.h and self.y + self.h >= hb.y):
            return True
        return False
    
class cactus:
    def __init__(self, x = 500, y = 400, w = 64, h = 64, speed = 0.2, hb = hitbox(508, 400, 48, 64)):
        self.x = x
        self.y = y
        self.ix = self.x
        self.iy = self.y
        self.w = w
        self.h = h
        self.speed = speed
        self.px = x
        self.hb = hb
        self.collide = 0
    def reset(self, spd):
        self.x = self.ix
        self.y = self.iy
        self.collide = 0
        self.hb.update(self.x + 8, self.y, self.hb.w, self.hb.h)
    def update(self, dino, ns = 0):
        if self.x < -64:
            if ns == 0:
                return False
            else:
                self.speed = ns
                self.x = 500
                return True
        self.px = self.x
        self.x -= self.speed
        self.hb.update(self.x + 8, self.y, self.hb.w, self.hb.h)
        if self.hb.collide(dino.hb) == True:
            dino.collide = 1
            self.collide = 1
        else:
            if self.collide == 1 and dino.collide == 1:
                self.collide = 0
                dino.collide = 0
        return True
    def draw(self, screen, cbox = 0):
        if cbox == 1:
            self.hb.draw(screen, self.collide)
        screen.blit(cactus_img,(self.x, self.y))

class dino:
    def __init__(self, x = 25, y = 400, w = 64, h = 64, yvel = 0, jvel = -0.5, grav = 0.001, hb = hitbox(26, 400, 58, 64)):
        self.x = x
        self.y = y
        self.ix = x
        self.iy = y
        self.w = w
        self.h = h
        self.yvel = yvel
        self.jvel = jvel
        self.grav = grav
        self.hb = hb
        self.fl = 0
        self.collide = 0
    def reset(self):
        self.x = self.ix
        self.y = self.iy
        self.yvel = 0
        self.collide = 0
        self.hb.update(self.x + 1, self.y, self.hb.w, self.hb.h)
    def update(self, jmp):
        if self.y == self.iy:
            if jmp == 1 and self.fl == 0:
                self.fl = 1
                self.yvel = self.jvel
        self.y += self.yvel
        self.yvel += self.grav
        if self.y > self.iy:
            self.fl = 0
            self.y = self.iy
            self.yvel = 0
        self.hb.update(self.x + 1, self.y, self.hb.w, self.hb.h)
    def draw(self, screen, cbox = 0):
        if cbox == 1:
            self.hb.draw(screen, self.collide)
        screen.blit(dino_img,(self.x, self.y))
        
class game:
    def __init__(self, rvals = [], dinos = 0, cactuss = 0):
        if dinos == 0:
            self.dino = dino()
        else:
            self.dino = dinos
        if cactuss == 0:
            self.cactus = cactus()
        else:
            self.cactus = cactuss
        self.rvals = rvals
        self.score = 0
        self.maxs = len(self.rvals)
        self.cs = 0
    def update(self, jmp):
        self.dino.update(jmp)
        if self.cactus.update(self.dino) == False:
            self.cactus.update(self.dino, self.rvals[self.cs])
            self.cs += 1
            if(self.cs == self.maxs - 1):
                return True
        self.score += 0.05
        return False
    def reset(self):
        self.dino.reset()
        self.cactus.reset(self.rvals[0])
        self.score = 0
        self.cs = 0
    def draw(self, screen, cbox = 0):
        self.dino.draw(screen, cbox)
        self.cactus.draw(screen, cbox)
    def scorecalc(self):
        if self.cactus.x != 500:
            self.score += (self.cactus.px - self.caactus.x) * 0.1

#----------------------------------------------------------------------------------------------player game

def rungame(game, screen):
    run = 1
    jmp = 0
    while run == 1:
        screen.fill((255, 255, 255))
        text = font.render(str(int(game.score)), True, (0, 0, 0))
        screen.blit(text, (50, 100))
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                run = 0
                return
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_SPACE:
                    jmp = 1
                if event.key == pygame.K_ESCAPE:
                    run = 0
                    break
            if event.type == pygame.KEYUP:
                if event.key == pygame.K_SPACE:
                    jmp = 0
        if game.update(jmp):
            print("Winner")
            run = 0
        game.draw(screen, 1)
        pygame.display.update()
        if game.cactus.collide == 1:
            game.reset()
            run = 0
        while run == 0:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    return
                if event.type == pygame.KEYUP:
                    if event.key == pygame.K_KP_ENTER or event.key == pygame.K_RETURN:
                        game.reset()
                        jmp = 0
                        run = 1

#-----------------------------------------------------------------------------------------AI game

def gennewnetw(disp = 0):
    l1 = layer()
    l1.newlayer(2, 4)
    l2 = layer()
    l2.newlayer(4, 4)
    ol = layer()
    ol.newlayer(4, 1, binac)
    netw = network(3, [l1, l2, ol])
    if disp == 1:
        netw.dispnw()
    return netw

def rungameai(game, netw = 0, screen = 0, fl = 1):
    #AI code
    #inp is distance, speed
    global rc
    game.reset()
    if netw == 0:
        netw = gennewnetw()
    run = 1
    jmp = 0
    while run == 1:
        inp = [(game.cactus.hb.x - (game.dino.hb.x + game.dino.hb.w)), game.cactus.speed]
        netw.runnetwork(inp)#distance, speed

        if game.update(netw.op[0]) == True:
            print("Winner")
            run = 0
        if screen != 0:
            pygame.event.get()
            screen.fill((255, 255, 255))
            text = font.render(str(int(game.score)), True, (0, 0, 0))
            screen.blit(text, (50, 100))
            game.draw(screen, 1)
            pygame.display.update()
        if game.cactus.collide == 1:
            run = 0
            if fl == 1:
                print("press to continue")
        if fl == 1:
            while run == 0:
                for event in pygame.event.get():
                    if event.type == pygame.QUIT:
                        return
                    if event.type == pygame.KEYUP:
                        if event.key == pygame.K_KP_ENTER or event.key == pygame.K_RETURN:
                            game.reset()
                            jmp = 0
                            run = 1
                        if event.key == pygame.K_ESCAPE:
                            time.sleep(1)
                            return
    return

def crossover(netw1, netw2 ,disp = 0):
    nlayer = netw1.numlayers
    maxc = 0
    lc = 0
    for i in netw1.layers:
        maxc += i.pl * i.cl
    rno = random.randrange(0, maxc)
    count = 0
    layers = []
    l1 = netw1.layers
    l2 = netw2.layers
    while lc < nlayer:
        ln1w = l1[lc].w
        ln2w = l2[lc].w
        p = l1[lc].pl
        c = l1[lc].cl
        w = []
        for i in range(c):
            sl = []
            for j in range(p):
                if count <= rno:
                    sl.append(ln1w[i][j])
                else:
                    sl.append(ln2w[i][j])
                count += 1
            w.append(sl)
        layers.append(layer(p, c, w, l1[lc].af))
        lc += 1
    newnet = network(nlayer, layers)
    if disp == 1:
        print("layer1")
        netw1.dispnw()
        print("layer2")
        netw2.dispnw()
        print("layer3")
        newnet.dispnw()
        print(rno)
    return newnet

def mutate(netw, rate):
    for i in netw.layers:
        for j in range(len(i.w)):
            for k in range(len(i.w[j])):
                lb = random.uniform(0, (1 - rate))
                n = random.random()
                if n >= lb and n <= lb + rate:
                    i.w[j][k] = random.uniform(-1, 1)
    

def crossoverandmutate(netwl, no):
    #select, crossover and mutate
    ts = 0
    for i in range (no):
        ts += netwl[i].score
    for i in range(no):
        netwl[i].sprob = netwl[i].score / ts
    random.shuffle(netwl)
    networklis = [[i,0,0] for i in netwl]
    networklis[0][2] = networklis[0][0].sprob
    for i in range(1, no):
        networklis[i][1] = networklis[i-1][2]
        networklis[i][2] = networklis[i][1] + networklis[i][0].sprob
    for i in range(10):
        num1 = random.random()
        num2 = random.random()
        if networklis[0][1] <= num1 and networklis[0][2] >= num1:
            netw1 = networklis[0][0]
        else:
            for i in range(1, no):
                if networklis[i][1] < num1 and networklis[i][2] >= num1:
                    netw1 = networklis[i][0]
                    break
        if networklis[0][1] <= num2 and networklis[0][2] >= num2:
            netw2 = networklis[0][0]
        else:
            for i in range(1, no):
                if networklis[i][1] < num2 and networklis[i][2] >= num2:
                    netw2 = networklis[i][0]
                    break
        newnet = crossover(netw1, netw2)
        mutate(newnet, mutation_r)
        netwl.append(newnet)
    

def ai(screen = 0, netwl = 0, games = 0, mt = 1):
    itr = 0
    if netwl == 0:
        netwl = [gennewnetw() for i in range(100)]
    no = len(netwl)
    if games == 0:
        games = [game(rvals) for i in range(no)]

    while True:
        for i in games:
            i.reset()
        if screen != 0 and mt == 0:
            for i in range(no):
                rungameai(games[i], netwl[i], screen, 0)
                netwl[i].score = games[i].score
                games[i].reset()
            
        else:
            if mt == 1:
                threads = []
                for i in range(no):
                    x = threading.Thread(target = rungameai, args = (games[i], netwl[i], 0, 0))
                    threads.append(x)
                    x.start()
                for i in range(no):
                    threads[i].join()
                    netwl[i].score = games[i].score
            else:
                for i in range(no):
                    rungameai(games[i], netwl[i], 0, 0)
                    netwl[i].score = games[i].score
        netwl.sort(key = lambda a:a.score, reverse = True)
        maxval = netwl[0].score
        fl = 0
        for i in netwl:
                if i.score > 2000:
                    fl = 1
                    break
        if fl ==1:
            break
        for i in range(10):
            netwl.pop()

        crossoverandmutate(netwl, no - 10)
        no = len(netwl)
        print("Iteration: ",itr)
        print("Highest score = ", maxval, "\n")
        itr += 1
 
    netwl.sort(key = lambda a:a.score, reverse = True)
    for i in range(no):
        print(netwl[i].score, "   ", netwl[i].sprob, "  ",i)
    if mt == 1:
        print(len(threads))
    print()
    return netwl
#-----------------------------------------------------------------------------------------Main
            
pygame.init()
screen = pygame.display.set_mode((500, 500))
pygame.display.set_caption("Dino Game")
icon = pygame.image.load("dinosaur.png")
dino_img= pygame.image.load("gamedino.png")
cactus_img= pygame.image.load("cactus.png")
font = pygame.font.SysFont('Arial', 20)
pygame.display.set_icon(icon)
rvals = [random.uniform(0.2,0.5) for i in range(100)]

a = game(rvals)
#pygame.display.iconify()
#rungame(a, screen)
#rungameai(a, screen = 0)
netwl = [gennewnetw() for i in range(100)]
games = [game(rvals) for i in range(100)]
nw = ai(0, netwl, mt = 0)
b = int(input("Enter a number"))
while(b != -1):
    if (b == -2):
        nw = ai(0, nw, games, 1)
    elif (b == -3):
        nw = ai(0, nw, games, 0)
    else:
        pygame.display.update()
        rungameai(a,nw[b],screen,1)
    b = int(input("Enter a number")) 

pygame.quit()
