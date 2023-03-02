
wurst = 0.3;

overlap = floor(5/wurst)*wurst;
overlapHeight = floor(1/wurst)*wurst;

hole = floor(48/wurst)*wurst;

holeHeight = floor(1/wurst)*wurst;

btn = floor((25)/wurst)*wurst + wurst;

// min winkel
$fa = 0.01;

// min size
$fs = wurst;

difference(){
  union(){
  
    // hole
    cylinder(h = holeHeight + overlapHeight, d = hole, center = false);

    // overlap
    cylinder(h = overlapHeight, d = hole + overlap, center = false);

  };
  
  translate([0,0,-3]) cylinder(h = holeHeight + overlapHeight + 6, d = btn, center = false);
 };