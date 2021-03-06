"""
.. module:: launch_sim.py
	:platform: MacOS, Unix, Windows,
	:synopsis: Simulation of launching projectile with a rail accelerator to 
    orbit
.. moduleauthor:: Pavlo Vlastos <pvlastos@ucsc.edu>
"""

##############################################################################
import RailAcceleration.Accelerator as AR
import RailAcceleration.Projectile.Projectile as PR
import RailAcceleration.Constants.Constants as CN
import RailAcceleration.Utilities.Rotations as ROT
import RailAcceleration.Modeling.Forces as FO
import numpy as np
#import scipy
# from scipy import integrate

import matplotlib
from matplotlib import cm
import matplotlib.pyplot as plt
from matplotlib import gridspec
from matplotlib.markers import MarkerStyle
import matplotlib.animation as animation

import argparse

##############################################################################
parser = argparse.ArgumentParser()

parser.add_argument('-d', '--dynamicGraphing', dest='dynamicGraphing', 
                    action='store_true', help='Graph the simulation as it \
                        calculates')

parser.add_argument('-f', '--force', type=float, dest='initforce', 
                    default=10000, help='Specify initial force from \
                        accelerator')

parser.add_argument('-i', '--interval', type=int, dest='interval',
                    default=200, help='Specify the graphing interval.')

parser.add_argument('-s', '--staticGraphing', dest='staticGraphing', 
                    action='store_true', help='Graph the simulation after it \
                        runs the main loop')

parser.add_argument('-v', '--verbose', dest='verbose', 
                    action='store_true', help='Display state at every \
                        iteration', default = False)

arguments = parser.parse_args()

dynamicGraphing = arguments.dynamicGraphing
initforce = arguments.initforce
graphInterval = arguments.interval
staticGraphing = arguments.staticGraphing
verbose = arguments.verbose

##############################################################################
# print("Building accelerator (Amy)...")
# Amy = AR.Accelerator()
# print("Finished building accelerator (Amy)")

print("Building {} object".format(PR.Projectile.__name__))
Jack = PR.Projectile()
print("Finished building {} object".format(PR.Projectile.__name__))

##############################################################################
# Setup Graphing
fig = plt.figure(figsize=(10,8))

gs = gridspec.GridSpec(nrows=2, ncols=6)
ax0 = fig.add_subplot(gs[0:2, 0:3], projection='3d')
fig.tight_layout(pad=4.0)

plt.show(block=False)

# 3D Plot
ax0.set_title('Projectile Position Over Time')

ax0.set_xlim(CN.xmin, CN.xmax)
ax0.set_ylim(CN.ymin, CN.ymax)
ax0.set_zlim(CN.zmin, CN.zmax)

ax0.view_init(80, -122)

ax0.set_xlabel("X (meters)")
ax0.set_ylabel("Y (meters)")
ax0.set_zlabel("Z (meters)")

ax0.grid()

# Record Projectile Position, TODO: velocity, acceleration, etc...
# Should be Mx1 vectors
x = np.array([Jack.state.x]) 
y = np.array([Jack.state.y])
z = np.array([Jack.state.z])

position = ax0.plot(x, y, z, lw=2, marker='.', color='red')[0]

# Intial launch vector
r = np.array([[Jack.state.x],
              [Jack.state.y],
              [Jack.state.z]])
r -= CN.origin

R = ROT.dcmFromEuler(-np.pi/2.0, 0.0, 0.0)

point = np.matmul(R, r) 

v = np.array([[1], 
              [0], 
              [0]])

u = r/np.linalg.norm(r)

print('u.shape={}'.format(u.shape))
print('v.shape={}'.format(v.shape))
print('point={}'.format(point))

w = np.cross(u.T, v.T)

angle = ROT.rotationAngleFrom2vectors(u, v)
print('angle.shape={}'.format(angle.shape))

print('w.shape={}'.format(w.shape))
print('angle={}'.format(angle))
R = ROT.dcmFromAngleAndVector(angle, w.T)

phi, theta, psi = ROT.EulerAnglesFromDCM(R)

Jack.state.setOrientation(phi, theta, psi)

r += CN.origin
p = point + r

ilvx = np.array([r[0][0],p[0][0]])
ilvy = np.array([r[1][0],p[1][0]])
ilvz = np.array([r[2][0],p[2][0]])

ilv = ax0.plot(ilvx[:], ilvy[:], ilvz[:], lw=3, marker='.', 
               color='black')[0]

# Sphere
u, v = np.mgrid[0:2*np.pi:20j, 0:np.pi:10j]
sphx = np.cos(u)*np.sin(v)*CN.radiusEarth + CN.origin[0][0]
sphy = np.sin(u)*np.sin(v)*CN.radiusEarth + CN.origin[1][0]
sphz = np.cos(v)*CN.radiusEarth + CN.origin[2][0]

sphere = ax0.plot_wireframe(sphx, sphy, sphz, color="blue")
# sphere = ax0.plot(sphx*np.nan, sphy*np.nan, sphz*np.nan, marker='.', color="blue")[0]

# Radius Vector
rx = np.array([CN.origin[0][0], r[0][0]])
ry = np.array([CN.origin[1][0], r[1][0]])
rz = np.array([CN.origin[2][0], r[2][0]])
radius = ax0.plot(rx[:], ry[:], rz[:], lw=3, marker='.', 
               color='lime')[0]

r -= CN.origin
r = r/np.linalg.norm(r) * CN.radiusEarth/2
r += CN.origin

forcex = np.array([Jack.state.x, r[0][0]])
forcey = np.array([Jack.state.y, r[1][0]])
forcez = np.array([Jack.state.z, r[2][0]])
force = ax0.plot(forcex[:], forcey[:], forcez[:], lw=3, marker='.', 
               color='orange')[0]

# Final Setup Steps
fig.canvas.draw()

background0 = fig.canvas.copy_from_bbox(ax0.bbox)

ax0.draw_artist(position)
ax0.draw_artist(radius)
ax0.draw_artist(force)
ax0.draw_artist(sphere)

fig.canvas.blit(ax0.bbox)

##############################################################################
# Main Loop Setup

# Timing
t = np.arange(CN.t0, CN.tf, CN.dt)
maxStepNum = len(t)
steps = range(0, maxStepNum)

# RK = scipy.integrate.RK45(Jack.model.update, CN.t0, Y0, CN.tf)
# print(RK)

#initial conditions
print('Jack.model.X=\r\n{}'.format(Jack.model.X.T))
print('Jack.model.Xshape=\r\n{}'.format(Jack.model.X.T.shape))
X = Jack.model.getX()
u = np.zeros(3)

# Main Loop
for k in steps:

    X = Jack.update(t[k], X, u)

    # scipy.integrate.RK45(Jack.model.update, t[k], Yi, t[k]+CN.dt)

    r = Jack.state.getPosVector()

    Fg = FO.calculateGravity(r, CN.massEarth, 
                             Jack.state.mass, CN.origin)

    if t[k] < CN.tl:
        Fi = point/np.linalg.norm(point)*initforce

    else:
        Fi = np.zeros((3,1))
    Fi = np.reshape(Fi, (len(Fi),))

    u = Fi + Fg

    # Graphing Update
    if (np.mod(k, graphInterval) == 0) and dynamicGraphing:
                
        # fig.canvas.restore_region(background0)

        x = np.concatenate((x, np.array([Jack.state.x])), axis=0) 
        y = np.concatenate((y, np.array([Jack.state.y])), axis=0)
        z = np.concatenate((z, np.array([Jack.state.z])), axis=0)
        if verbose:
            print("k = {}, Fgmag = {}, Fimag = {}, x:{}, y:{}, z:{}, mass:{}"
                .format(k,  round(np.linalg.norm(Fg)), 
                round(np.linalg.norm(Fi)), round(Jack.state.x), 
                round(Jack.state.y), round(Jack.state.z), Jack.state.mass))
        
        position.set_data(x, y)
        position.set_3d_properties(z)
        
        r += CN.origin
        rx = np.array([CN.origin[0][0], r[0][0]])
        ry = np.array([CN.origin[1][0], r[1][0]])
        rz = np.array([CN.origin[2][0], r[2][0]])

        radius.set_data(rx, ry)
        radius.set_3d_properties(rz)

        a = np.array([[Jack.state.x],[Jack.state.y],[Jack.state.z]])
        a -= CN.origin
        a = a/np.linalg.norm(a) * CN.radiusEarth
        a += CN.origin

        forcex = np.array([Jack.state.x, a[0][0]])
        forcey = np.array([Jack.state.y, a[1][0]])
        forcez = np.array([Jack.state.z, a[2][0]])

        force.set_data(forcex, forcey)
        force.set_3d_properties(forcez)

        ax0.draw_artist(position)
        ax0.draw_artist(radius)
        ax0.draw_artist(force)
        ax0.draw_artist(sphere)

        fig.canvas.blit(ax0.bbox)

        fig.canvas.flush_events()

# if staticGraphing:
