/**
 * Copyright (c) 2015 Guilhelm Savin. All rights reserved.
 *
 * This file is part of the ARDUICIOLE project.
 *
 * ARDUICIOLE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ARDUICIOLE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ARDUICIOLE.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Paramètres de l'imprimante :
 * - extrusion width        : 0.40mm
 * - infill extrusion width : 0.43mm
 * - layer thickness        : 0.25mm
 * - infill                 : 100% (quasiment pas de solide, en dehors des bords. Les rendre plein ne change pas grand chose au poids mais augmente la résistance=
 * - speed                  : 20mm/s
 * - main temp              : 200°C
 * - 1st layer temp         : 205°C
 * - support                : off, mais garde le "raft" en "skirt" pour dégager le surplus dans l'extruder au démarrage.
 */

sep=1;
gap=4;
inter_gap=1.5;
bord=4;

grille_w=1.5;

// Batterie HF5X :
//  battery_w=42;
//  battery_h=66;

// Batterie Samsung :
battery_w=46.25;
battery_h=59+sep+4;
battery_d=5;

arduino_w=18.75;
arduino_h=33.8;
arduino_d=3;

xbee_w=31;
xbee_h=26;
xbee_support_w=28;
xbee_support_d=2;
xbee_d=2;

hled_w=25.5;
hled_h=25.5;

socle=2;
socle_int_w=battery_w+xbee_w+3*sep;
socle_int_h=battery_h+2*sep;
socle_w=socle_int_w+2*bord;
socle_h=socle_int_h+2*bord;
socle_d=2;

delta = 2;
$fn=20;

echo(socle_w, socle_h);

//
// Emplacements.
// (0, 0) est le centre. Les coordonnées sont ceux du coin SW.
//

battery_x=-socle_int_w/2;
battery_y=-socle_int_h/2;

arduino_x=socle_int_w/2-arduino_w-2*sep;
arduino_y=-socle_int_h/2;

xbee_x=socle_int_w/2-xbee_w-2*sep;
xbee_y=socle_int_h/2-xbee_h-2*sep;

hled_x=socle_int_w/2-2*sep-arduino_w-hled_w;
hled_y=socle_int_h/2-xbee_h-2*sep-hled_h;

module bottom() {
    translate([-socle_w/2, -socle_h/2, 0])
    difference() {
        cube([socle_w, socle_h, socle_d]);

        translate([bord, bord, -1])
        cube([socle_int_w, socle_int_h, 7]);
    }
}

//
// Crée le contour d'un rectangle
//

module bb(w, h, d, e=sep) {
    difference() {
        cube([w+2*e, h+2*e, d+socle]);

        translate([e, e, -1])
        cube([w, h, d+socle+2]);
    }
}

module battery_box(inv=false, d=5) {
    union() {
        difference() {
            union() {
                bb(battery_w, battery_h, d);

                //
                // Grille de support
                //

                for (i = [0:13:battery_h-5])
                translate([sep, sep+i, 0])
                cube([battery_w, grille_w, socle_d+(inv?0:2)]);

                if (!inv) {
                  //
                  // Fin de la batterie
                  //

                  translate([sep, sep+59, 0])
                  cube([battery_w, sep, d+socle]);

                  //
                  // Support pour les connecteurs.
                  //
                  union() {
                    translate([sep+7.25, sep+59+sep, 0])
                    cube([3*2.54+1, 4, socle_d+2+1.25]);

                    translate([sep+7.25, sep+59+sep+2.5, socle_d+2])
                    cube([3*2.54+1, 1.5, 1.25+1.25]);
                  }
                }
            }

            //
            // Fentes des connecteurs de la batterie
            //

            if(!inv)
            for (i = [0:3])
            translate([sep+7.25+i*2.54, socle_int_h-7, socle_d+3+1.25])
            cube([1, 9, battery_d]);

            if(inv) {
                //
                // Un peu de place pour les connecteurs de la batterie
                //

                translate([sep+5.5, socle_int_h-2, socle+battery_d-1.5])
                cube([9.5, 6, 5]);

                //
                // Vide pour le trou du couvercle
                //

                translate([2*gap, 2*gap, -1])
                cube([battery_w-4*gap, battery_h-4*gap, d+2]);
            }
        }

        //
        // Trou du couvercle
        //

        if (inv)
        difference() {
            translate([2*gap, sep+2*gap+inter_gap, 0])
            cube([battery_w-4*gap, 8*gap+9*inter_gap, socle_d]);

            translate([2*gap+inter_gap, sep+2*gap+2*inter_gap, -1])
            cube([battery_w-4*gap-2*inter_gap, 8*gap+7*inter_gap, socle_d+2]);
        }
    }
}

module arduino_box(inv=false, d=5) {
    union() {
        bb(arduino_w, arduino_h, d);

        //
        // Grille de support
        //

        difference() {
            union() {
                for (i = [3:12.5:arduino_h-2])
                translate([sep, sep+i, 0])
                cube([arduino_w, grille_w, socle_d+(inv?0:1)]);
            }

            if(!inv) {
                translate([sep, 0, socle_d-0.5])
                cube([3, arduino_h, +3]);
                translate([sep+arduino_w-3, 0, socle_d-0.5])
                cube([3, arduino_h, +3]);
            }
        }
    }
}

module xbee_box(inv=false, d=5) {

    union() {
        bb(xbee_w, xbee_h, d);

        //
        // Grille de support
        //

        translate([sep, sep+4, 0])
        cube([xbee_w, grille_w, socle_d]);

        translate([sep+(inv?0:6+grille_w), sep+xbee_h/2-grille_w/2, 0])
        cube([(inv?xbee_w:xbee_w-6-grille_w), grille_w, socle_d]);

        translate([sep, sep+xbee_h-grille_w-4, 0])
        cube([xbee_w, grille_w, socle_d]);

        //
        // Butée pour le support
        //

        if(!inv) {
            for (i=[4,xbee_h-6])
            translate([sep+xbee_support_w, sep+i, socle_d])
            cube([xbee_w-xbee_support_w, 2, 1.5]);
        }

        //
        // Espace pour les soudures
        //

        if(!inv)
        translate([sep+6, sep+4+grille_w, 0])
        cube([grille_w, xbee_h-8-2*grille_w, socle_d]);
    }
}

module interior(inv=false) {
    difference() {
        //
        // Les différents emplacements
        //

        union() {
            translate([battery_x, battery_y, 0])
            battery_box(inv);

            translate([arduino_x, arduino_y, 0])
            arduino_box(inv);

            translate([xbee_x, xbee_y, 0])
            xbee_box(inv);
        }

        //
        // On enlève un peu de plastique pour alléger et permettre le passage des câbles
        //

        for (i = [10,22, 34, 47, 56])
        translate([-socle_w/2, -socle_h/2+i-2.5, 7-2.5])
        cube([socle_w, 5, 5]);

        for (i = [25, 34, 43, 55, 69, 79])
        translate([-socle_w/2+i-2.5, -socle_h/2, 7-2.5])
        cube([5, , socle_h, 5]);
    }
}

module interior_inv() {
    mirror() {
        interior(true);
    }
}

module hled() {
    union() {
        cube([25,25,2]);

        translate([3, 3, -2])
        cube([19, 19, 2]);

        translate([19, 8.5, 2])
        cube([6, 8, 12]);
    }
}

module part1() {
  difference() {
    union() {
        difference() {
            union() {
                bottom();
                interior();
            }

            //
            // Vide pour la LED puissante
            //

            translate([hled_x, hled_y, -1])
            cube([hled_w, hled_h, 20]);
        }

        //
        // Support de la LED puissante
        //

        translate([hled_x, hled_y, 0]) {
            difference() {
                cube([hled_w, hled_h, 1]);

                translate([3, 3, -1])
                cube([hled_w-6, hled_h-6, 3]);
            }
        }

        //
        // Boucle d'attaches
        //

        for (p = [[-socle_w / 2, -socle_h / 2], [-socle_w / 2, socle_h / 2], [socle_w / 2, -socle_h / 2], [socle_w / 2, socle_h / 2]])
        translate([p.x + (p.x < 0 ? delta : -delta), p.y + (p.y < 0 ? delta : -delta), 0])
        linear_extrude(socle)
        circle(3);
    }

    for (p = [[-socle_w / 2, -socle_h / 2], [-socle_w / 2, socle_h / 2], [socle_w / 2, -socle_h / 2], [socle_w / 2, socle_h / 2]])
    translate([p.x + (p.x < 0 ? delta : -delta), p.y + (p.y < 0 ? delta : -delta), -1])
    linear_extrude(socle+2)
    circle(1.5);
  }
}

module part2() {
    translate([-socle_w/2, -socle_h/2, 0])
    difference() {
        union() {
            difference() {
                cube([socle_w, socle_h, socle_d]);

                translate([bord, bord, -1])
                cube([socle_int_w, socle_int_h, socle_d+2]);
            }

            translate([bord/2, bord/2, socle_d])
            difference() {
                //
                // Matière première pour les bords
                //

                cube([socle_w-bord, socle_h-bord, 10]);

                //
                // On vide l'intérieur
                //

                translate([bord/2, bord/2, -1])
                cube([socle_int_w, socle_int_h, 12]);

                //
                // On enlève le plastique des milieux
                //

                translate([-bord/2, arduino_w-bord/2-2, -1])
                cube([socle_w, socle_h-2*arduino_w+4, 12]);

                translate([arduino_w-bord/2, -bord/2, -1])
                cube([socle_w-2*arduino_w, socle_h, 12]);
            }

            //
            // Boucle d'attaches
            //

            for (p = [[bord, bord], [bord, socle_h], [socle_w, bord], [socle_w, socle_h]])
            translate([p.x + (p.x < 0 ? delta : -delta), p.y + (p.y < 0 ? delta : -delta), 0])
            linear_extrude(socle)
            circle(3);
        }

        for (p = [[bord, bord], [bord, socle_h], [socle_w, bord], [socle_w, socle_h]])
        translate([p.x + (p.x < 0 ? delta : -delta), p.y + (p.y < 0 ? delta : -delta), -1])
        linear_extrude(socle+20)
        circle(1.5);
    }

    interior_inv();
}
