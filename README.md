# 1601_ProjectSoftware

![image](https://github.com/AbacusIsMad/1601_ProjectSoftware/assets/101756598/5d1df27b-296b-4040-898c-cd0af2c4133a)

The robot pathfinding algorithm isn't that good, but the code has several improvements:
- Collision detection is now done by querying a bitmap, and that means you can **draw** your map instead of writing a linked list!
    - Any pixel with alpha value `>0` is considered "solid"
- Added a separate mode which mathematically reproduces a differential driving system (turn on with `C`, off with `V`)
    - While in this mode use `Q` and `A` to control the left wheel and `E` `D` to control the others
    - Otherwise use `UP` `DOWN` `LEFT` `RIGHT`
- Added (semi transparent) breadcrumbs to where the robot has been
- Added a primitive tangent calculation and thus the angle of incidence of sensors hitting the wall, then visualising it (green + red lines above)
- Added a speed and direction bar
