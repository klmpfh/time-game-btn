
initial_val = 102.5;

hbt = [initial_val, initial_val/3*4, initial_val/4*2];
wand = initial_val * 2 / 100;
bottom_steg = wand; //wand*2 ;//wand * 5;

display = [61,15,wand*2];

btn_radius = 25;
btn_mutter = 5;

cable_radius = 10 / 2;
calbe_nut = 14 / 2;

kabelbinder = 3;

echo([
    hbt[0] - (wand*4),
    hbt[1] - (wand*4),
    hbt[2] - (wand*3)
]);
    
if((hbt[0] - (wand*4)) < 90) echo("!!!!!!!!!!! zu schmal");
if((hbt[2] - (wand*3)) < 45) echo("!!!!!!!!!!! zu klein (höhe)");

rotate([0,180,0]){

    difference(){
        
        // main cube
        roundedcube(hbt, false, wand, "zmax");

        // not filled
        translate([wand,wand,wand])
        roundedcube([
            hbt[0] - (wand*2),
            hbt[1] - (wand*2),
            hbt[2] - (wand*2)
        ], false, wand * 2);
        
        
        // bottom
        translate([bottom_steg,bottom_steg,-hbt[2]/2])
        roundedcube([
            hbt[0] - 2*bottom_steg,
            hbt[1] - 2*bottom_steg,
            hbt[2]
        ], false, wand * 2);

        // display cube
        translate([
            (hbt[0] - display[0]) / 2,
            hbt[1] - hbt[1]/3 + display[1]/2,
            hbt[2] - (wand*1.5)
        ])
        cube(display,false);

        // btn hole
        translate([
            hbt[0]/2,
            hbt[1]/3,
            hbt[2]-wand*2
        ])
        cylinder(h=wand*2, r=btn_radius, center=false);
        
        // cabel
        translate([
            hbt[0]/4,
            hbt[1],
            wand*3 + calbe_nut
        ])
        rotate([90,0,0])
        cylinder(h=wand*2,r=cable_radius,center=false);
        
        
        
        // füße
        translate([4*wand,-wand,0])
        hull(){
        
            rotate([-90,0,0]){
                
                cylinder(h = 2*wand + hbt[1], r = wand, center = false);
                translate([hbt[0]-8*wand,0,0])cylinder(h = 2*wand + hbt[1], r = wand, center = false);
            }
        
        }
        
        rotate([180,0,90])
        translate([4*wand,-wand,0])
        hull(){
        
            rotate([-90,0,0]){
                
                cylinder(h = 2*wand + hbt[1], r = wand, center = false);
                translate([hbt[1]-8*wand,0,0])cylinder(h = 2*wand + hbt[1], r = wand, center = false);
            }
        
        }
        

    }

    // stützen der decke
    translate([
        0,
        hbt[1]/3,
        0
    ])
    translate([
        0,
        btn_radius + btn_mutter, // paltz für mutter
        hbt[2] - wand*3
    ])
    cube([
        hbt[0],
        wand,
        wand*2
    ], false);

    translate([
        0,
        hbt[1]/3,
        0
    ])
    translate([
        0,
        -btn_radius - wand - btn_mutter, // paltz für mutter
        hbt[2] - wand*3
    ])
    cube([
        hbt[0],
        wand,
        wand*2
    ], false);
    
}

$fs = 0.01;

module roundedcube(size = [1, 1, 1], center = false, radius = 0.5, apply_to = "all") {
	// If single value, convert to [x, y, z] vector
	size = (size[0] == undef) ? [size, size, size] : size;

	translate_min = radius;
	translate_xmax = size[0] - radius;
	translate_ymax = size[1] - radius;
	translate_zmax = size[2] - radius;

	diameter = radius * 2;

	obj_translate = (center == false) ?
		[0, 0, 0] : [
			-(size[0] / 2),
			-(size[1] / 2),
			-(size[2] / 2)
		];

	translate(v = obj_translate) {
		hull() {
			for (translate_x = [translate_min, translate_xmax]) {
				x_at = (translate_x == translate_min) ? "min" : "max";
				for (translate_y = [translate_min, translate_ymax]) {
					y_at = (translate_y == translate_min) ? "min" : "max";
					for (translate_z = [translate_min, translate_zmax]) {
						z_at = (translate_z == translate_min) ? "min" : "max";

						translate(v = [translate_x, translate_y, translate_z])
						if (
							(apply_to == "all") ||
							(apply_to == "xmin" && x_at == "min") || (apply_to == "xmax" && x_at == "max") ||
							(apply_to == "ymin" && y_at == "min") || (apply_to == "ymax" && y_at == "max") ||
							(apply_to == "zmin" && z_at == "min") || (apply_to == "zmax" && z_at == "max")
						) {
							sphere(r = radius);
						} else {
							rotate = 
								(apply_to == "xmin" || apply_to == "xmax" || apply_to == "x") ? [0, 90, 0] : (
								(apply_to == "ymin" || apply_to == "ymax" || apply_to == "y") ? [90, 90, 0] :
								[0, 0, 0]
							);
							rotate(a = rotate)
							cylinder(h = diameter, r = radius, center = true);
						}
					}
				}
			}
		}
	}
}