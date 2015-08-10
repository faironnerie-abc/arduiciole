sep=1;
gap=4;
inter_gap=1.5;
bord=4;

battery_w=42;
battery_h=66;
battery_d=5;

arduino_w=18;
arduino_h=33.5;
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
                
                for (i = [0:10:battery_h-5])
                translate([sep, sep+i, 0])
                cube([battery_w, 2, socle_d+(inv?0:2)]);
               
                //
                // Support pour les connecteurs.
                //
                
                if (!inv)
                translate([5, socle_int_h-2-sep, 0])
                cube([13, 2, socle_d+2]);
            }
            
            //
            // Fentes des connecteurs de la batterie
            //
            
            for (i = [0:3])
            translate([sep+5.5+i*3, socle_int_h-2, socle_d+2])
            cube([0.5, 6, battery_d]);
            
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
        
        for (i = [3:8:arduino_h-2])
        translate([sep, sep+i, 0])
        cube([arduino_w, 2, socle_d]);
    }
}

module xbee_box(inv=false, d=5) {
    union() {
        bb(xbee_w, xbee_h, d);
    
        //
        // Grille de support
        //
        
        translate([sep, sep+4, 0])
        cube([xbee_w, 2, socle_d]);
        
        translate([sep+(inv?0:7), sep+xbee_h/2-1, 0])
        cube([xbee_w-(inv?0:7), 2, socle_d]);
        
        translate([sep, sep+xbee_h-2-4, 0])
        cube([xbee_w, 2, socle_d]);
        
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
        translate([sep+5, 6+sep, 0])
        cube([2, xbee_h-12, socle_d]);
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
        
        for (i = [10,22, 34, 49, 58, 67])
        translate([-socle_w/2, -socle_h/2+i-2.5, 7-2.5])
        cube([socle_w, 5, 5]);
        
        for (i = [25, 34, 43, 52.5, 66, 74])
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
        }
    }
    
    interior_inv();
}
