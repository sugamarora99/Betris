# Betris
EE319k Final Project - Embedded Game Design

# Overview
This game was created by Sugam Arora and Joonha Park for EE 319k: Introduction to Embedded Systems. The course was taught during Spring 2020 by Dr. Jon Valvano. The purpose of the project is to put the multiple concepts taught in the course together to create a working embedded system.

# How to Play
Betris is a clone of Tetris, the classic tile matching game. There are 7 different pieces, each of different shape and color. Each shape is made of 4 blocks. Once a piece is spawned, the player must decide how to place it in the grid by either rotating it (using one of the buttons) or moving it left to right (with the slide potentiometer). The player can also speed up the current piece's decent by holding down a button. As soon as the current piece either reaches the bottom of the grid or its bottom side is touching another block, the player can no longer move that piece and a new one will spawn. Once a row is completely filled with blocks, the row is deleted and the player's score is incremented. If there is a block in the position where new pieces spawn, the player loses and the game ends.
