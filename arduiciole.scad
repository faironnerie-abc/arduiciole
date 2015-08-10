include <arduiciole_parts.scad>;

translate([0, 0, 0])
part1();

translate([0, 0, 50])
rotate([0, 180, 0])
part2();

color("green") {
    translate([hled_x, hled_y, 10])
    hled();
    
    translate([battery_x+sep, battery_y+sep, 30])
    cube([battery_w, battery_h, battery_d]);
    
    translate([arduino_x+sep, arduino_y+sep, 15])
    cube([arduino_w, arduino_h, arduino_d]);
    
    translate([0, 0, 20]) {
        translate([xbee_x+sep, xbee_y+sep, 0])
        cube([xbee_support_w, xbee_h, xbee_support_d]);
        
        translate([xbee_x+5+sep, xbee_y+sep, 8])
        cube([xbee_w-5, xbee_h, xbee_support_d]);
        
        for (i=[0,xbee_h-2])
        translate([xbee_x+sep+5, xbee_y+sep+i, 2])
        cube([21, 2, 6]);
    }
}
