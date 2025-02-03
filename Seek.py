import pygame
import math
import time

# Initialisation de Pygame
pygame.init()

class Vehicle:
    def __init__(self, mass, position, velocity, max_force, max_speed):
        self.mass = mass  # Masse du véhicule - Float
        self.position = pygame.math.Vector2(position)  # Position sous forme de vecteur - Vector 2
        self.velocity = pygame.math.Vector2(velocity)  # Vitesse sous forme de vecteur - Vector 2
        self.max_force = max_force  # Force maximale applicable - Float
        self.max_speed = max_speed  # Vitesse maximale autorisée - Float

def seek():
    desired_velocity = (target_pos - vehicle.position).normalize() * vehicle.max_speed
    return desired_velocity - vehicle.velocity

def flee():
    desired_velocity = (target_pos - vehicle.position).normalize() * vehicle.max_speed
    return -(desired_velocity - vehicle.velocity)

def pursuit():
    T = (target_pos - vehicle.position).length() / vehicle.max_speed
    futurePosition = target_pos + pygame.math.Vector2(pygame.mouse.get_rel()) * T
    desired_velocity = (futurePosition - vehicle.position).normalize() * vehicle.max_speed
    return desired_velocity - vehicle.velocity

def evasion():
    T = (target_pos - vehicle.position).length() / vehicle.max_speed
    futurePosition = target_pos + pygame.math.Vector2(pygame.mouse.get_rel()) * T
    desired_velocity = (futurePosition - vehicle.position).normalize() * vehicle.max_speed
    return -(desired_velocity - vehicle.velocity)
        
# Liste des points formant le chemin
path_points = [
    pygame.math.Vector2(100, 100),
    pygame.math.Vector2(200, 300),
    pygame.math.Vector2(400, 200),
    pygame.math.Vector2(300, 400),
    pygame.math.Vector2(100, 350),
]

# Indice du point actuellement visé
current_target_index = 0

# Constantes
WIDTH, HEIGHT = 500, 500
FPS = 60
STEP_TIME = 1.0 / FPS
slowing_distance = 100

# Création de la fenêtre
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Vehicle Steering Simulation")

# Chargement et transformation de l'image
image_bee = pygame.image.load("Bee.jpeg").convert_alpha()
image_bee = pygame.transform.scale(image_bee, (50, 50))

image_flower = pygame.image.load("Flower.jpg").convert_alpha()
image_flower = pygame.transform.scale(image_flower, (60, 60))

# Création du véhicule
vehicle = Vehicle(10, (100, 100), (0.2, 0.2), 1, 1)

steering_library = "seek"

running = True
prev_time = time.perf_counter()

while running:
    # Gestion du temps
    current_time = time.perf_counter()
    dt = current_time - prev_time
    prev_time = current_time
        
    # Gestion des événements
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
            
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_UP:
                steering_library = "seek"
            elif event.key == pygame.K_DOWN:
                steering_library = "flee"
            elif event.key == pygame.K_LEFT:
                steering_library = "pursuit"
            elif event.key == pygame.K_RIGHT:
                steering_library = "evasion"
            elif event.key == pygame.K_a:
                steering_library = "arrival"

    # Mise à jour de l'affichage
    screen.fill((255, 255, 255))
    
    # Définition de la cible (position de la souris)
    target_pos = pygame.math.Vector2(pygame.mouse.get_pos())
    #pygame.draw.circle(screen, (255, 255, 0), target_pos, 20)  # Dessiner la cible
    rect = image_flower.get_rect(center=target_pos)
    screen.blit(image_flower, rect.topleft)

    pygame.draw.circle(screen, (255, 255, 0), path_points[0], 20)
    pygame.draw.circle(screen, (255, 255, 0), path_points[1], 20)
    pygame.draw.circle(screen, (255, 255, 0), path_points[2], 20)
    pygame.draw.circle(screen, (255, 255, 0), path_points[3], 20)
    pygame.draw.circle(screen, (255, 255, 0), path_points[4], 20)

    '''if (steering_library == "seek"):
        steering = seek()
    elif (steering_library == "flee"):
        steering = flee()
    elif (steering_library == "pursuit"):
        steering = pursuit()
    elif (steering_library == "evasion"):
        T = (target_pos - vehicle.position).length() / vehicle.max_speed
        futurePosition = target_pos + pygame.math.Vector2(pygame.mouse.get_rel()) * T
        desired_velocity = (futurePosition - vehicle.position).normalize() * vehicle.max_speed
        steering = -(desired_velocity - vehicle.velocity)
    elif (steering_library == "arrival"):
        target_offset = target_pos - vehicle.position
        distance = target_offset.length()

        # Calcul de la vitesse en fonction de la distance
        if distance < slowing_distance:
            ramped_speed = vehicle.max_speed * (distance / slowing_distance)
            clipped_speed = min(ramped_speed, vehicle.max_speed)
        else:
            clipped_speed = vehicle.max_speed  # Garde la vitesse max si en dehors de slowing_distance

        # Calcul de la vitesse désirée
        desired_velocity = target_offset.normalize() * clipped_speed

        # Calcul du steering
        steering = desired_velocity - vehicle.velocity
        
    else:'''
    # Récupérer le point cible actuel
    target_pos = path_points[current_target_index]

    # Distance entre le véhicule et la cible
    target_offset = target_pos - vehicle.position
    distance = target_offset.length()

    # Se déplacer vers la cible (utilisation de Arrival pour un mouvement fluide)
    slowing_distance = 50  # Ajuste selon ton besoin

    if distance < slowing_distance:
        ramped_speed = vehicle.max_speed * (distance / slowing_distance)
        clipped_speed = min(ramped_speed, vehicle.max_speed)
    else:
        clipped_speed = vehicle.max_speed

    if distance > 0:  # Vérifie si la distance est non nulle avant de normaliser
        desired_velocity = target_offset.normalize() * clipped_speed
    else:
        desired_velocity = pygame.math.Vector2(0, 0)  # Pas de mouvement si déjà sur la cible
    steering = desired_velocity - vehicle.velocity

    # Appliquer la force de steering
    steering.clamp_magnitude_ip(vehicle.max_force)
    steering /= vehicle.mass

    # Mise à jour de la vitesse et de la position
    vehicle.velocity += steering
    vehicle.velocity.clamp_magnitude_ip(vehicle.max_speed)
    vehicle.position += vehicle.velocity

    # Vérifier si on est proche du point cible pour passer au suivant
    if distance < 10:  # Rayon de tolérance
        current_target_index = (current_target_index + 1) % len(path_points)  # Boucle infinie
        
    
    # Appliquer la force de steering (avec limite)
    '''steering.clamp_magnitude_ip(vehicle.max_force)
    steering /= vehicle.mass
    
    # Mise à jour de la vitesse et de la position
    vehicle.velocity += steering
    vehicle.velocity.clamp_magnitude_ip(vehicle.max_speed)
    vehicle.position += vehicle.velocity'''
    
    # Calcul de l'angle de rotation
    angle = math.degrees(math.atan2(-vehicle.velocity.y, vehicle.velocity.x))
    
    # Rotation et centrage de l'image
    rotated_image_bee = pygame.transform.rotate(image_bee, angle)
    new_rect = rotated_image_bee.get_rect(center=vehicle.position)
    screen.blit(rotated_image_bee, new_rect.topleft)

    pygame.display.flip()
    
    # Régulation du temps de boucle
    time.sleep(max(0, STEP_TIME - (time.perf_counter() - current_time)))

pygame.quit()
