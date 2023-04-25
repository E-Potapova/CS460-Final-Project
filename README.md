# Procedurally-generated Height Map from Perlin Noise

### Elizabeth Potapova & Tyler Ramos

### Problem Statement:
Procedural generation is used almost everywhere in computer graphics, including video games, animation, and film. It’s used to make many effects look more realistic by adding in a random factor to a texture or a movement.  


### Objective:
We wanted to look at the origins of procedurally-generated noise by implementing Perlin noise, the most popular method in generating noise, in openGL. From the generated noise, we want to create a simple voxel landscape by placing cubes at various heights depending on the colors/values within the generated noise map. We’re hoping to produce small cross sections of voxel terrain using this randomly generated noise map.


### Things we need to do:
- Generate Perlin noise
    - Read research paper on Perlin noise generation
    - Show on screen (in its own window)
- Show voxel landscape
    - Dimensions match the dimensions of the Perlin noise (square?)
    - Height of each section depends on color from Perlin noise
        - White (1.0) - very tall
        - Black (0.0) - very deep/low
    - Have landscape slowly rotate to see from all sides
    - Fixed camera position from above looking down
