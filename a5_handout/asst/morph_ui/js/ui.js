function getColorValue(colorIndex) {
    // random colors, change numColors in loadColorPicker when adding/substracting colors
    //var colors = ["#000", "#3FBF3F", "#1B1EED", "#C31BED", "#F7070B", "#FFFF00", "#fff"];
    var colors = ["#000", "#4bef4c", "#1B1EED", "#C31BED", "#F7070B", "#FFFF00", "#fff"];
    return colors[colorIndex];
}

function createColor(colorIndex) {
    var $div = $("<div>", {"class": "color-box", "id": "color-"+colorIndex});
    $div.css("background-color", getColorValue(colorIndex));
    $div.click(function(){
        if (!$(this).hasClass("color-selected")) {
            $(".color-selected").removeClass("color-selected");
            $(this).addClass("color-selected");
        }
    });
    return $div;
}

function loadColorPicker() {
    var numColors = 7;
    for (var i=0; i<numColors; i++) {
        $("#segment-color-picker").append(createColor(i));
    }
    // select red as the default start 
    $("#color-4").click();
}


function getSelectedColorValueVariable(lineNum) {
  //variable color with each line (can be a bit confusing)
  var col = getSelectedColorValue();
  var rgb = Raphael.getRGB(col);
  var hsb = Raphael.rgb2hsb(rgb.r, rgb.g, rgb.b);
  var clow = 0.09; // ~10 colors
  if(hsb.b < 0.5) {
    var delta = clow;
  } else{
    var delta = -clow;
  }
 
  var v = hsb.b + delta * lineNum % 1.;
  if (v < clow) {
      v = hsb.b;
  } 
  return Raphael.hsb2rgb(hsb.h, hsb.s, v);
}

function getSelectedColorValue() {
    var idOffset = "color-".length;
    return getColorValue($(".color-selected").attr("id").substring(idOffset));
}

function writeCppSegment(s, Px,Py,Qx,Qy, segNum) {
    s = s + ".push_back(Segment(Vec2f("+Px+", "+Py+"), Vec2f("+Qx+
    ", "+Qy+")));";
    return $("<span>", {class: "code-seg"}).text(s);
    // return s;
}

function getPos(e) {
    var posx = 0;
    var posy = 0;
    if (!e) e = window.event;
    if (e.pageX || e.pageY) 	{
        posx = e.pageX;
        posy = e.pageY;
    }
    else if (e.clientX || e.clientY) 	{
        posx = e.clientX + document.body.scrollLeft +
            document.documentElement.scrollLeft;
        posy = e.clientY + document.body.scrollTop +
            document.documentElement.scrollTop;
    }
    posx = Math.round(posx - offset.left);
    posy = Math.round(posy - offset.top);

    return [posx,posy];
}

$('document').ready( function() {
    loadColorPicker();

    imagewidth  = 0;
    imageheight = 0;
    cornerx1=10;
    cornery=10;

    var img = new Image();
    img.src = path1;
    img.onload = function() {
        imagewidth  = img.width;
        imageheight = img.height;
        console.log(imagewidth, imageheight);

        column_width = Math.max(imageheight*1.5, 500);
        cornerx2= cornerx1+column_width;
        
        //paper = Raphael(document.getElementById("canvas"), 2*column_width, imageheight);
        paper = Raphael(document.getElementById("canvas"), 2*(column_width + cornerx1), cornery + imageheight);
        //paper = Raphael(document.getElementById("canvas"), cornerx1+space+imagewidth1+imagewidth2, cornery+Math.max(imageheight1, imageheight2));

        lastx1=0;
        lasty1=0;
        count1=0;
        var selectedPoints1 = [];

        lastx2=0;
        lasty2=0;
        count2=0;
        var selectedPoints2 = [];


        offset = ($("#canvas").offset());
        console.log(offset.left);
        console.log(offset.top);
        $("#seg1_cell").width(column_width);

        function draw1(e) {
            var pos = getPos(e);
            var posx = pos[0];
            var posy = pos[1];
            //segment start = circle, end = rectangle
            if (count1%2>0) {
              //var ci=paper.rect(posx-4,posy-4,8,8).attr({fill: getSelectedColorValueVariable(count1 - 1)});
              var ci=paper.rect(posx-4,posy-4,8,8).attr({fill: getSelectedColorValue()});
            } else {
              //var ci=paper.circle(posx,posy, 5).attr({fill: getSelectedColorValueVariable(count1)});
              var ci=paper.circle(posx,posy, 5).attr({fill: getSelectedColorValue()});
            }
            if (count1%2>0) {
                //var path = paper.path("M"+lastx1+" "+lasty1+" L"+posx+" "+posy)
                  //.attr({stroke: getSelectedColorValueVariable(count1 - 1)});
                var path = paper.path("M"+lastx1+" "+lasty1+" L"+posx+" "+posy)
                  .attr({stroke: getSelectedColorValue()});
                var $segmentSpan = writeCppSegment("segsBefore", lastx1-cornerx1, lasty1-cornery, posx-cornerx1, posy-cornery);
                var $break = $("<br>");
                $("#seg1").append($segmentSpan).append($break);

                // highlight corresponding line of code on hover of the segment
                ci.hover(function() {
                    $segmentSpan.addClass("selected-segment");
                }, function() {
                    $segmentSpan.removeClass("selected-segment");
                });
                ci.prev.hover(function() {
                    $segmentSpan.addClass("selected-segment");
                }, function() {
                    $segmentSpan.removeClass("selected-segment");
                });
                path.hover(function() {
                    $segmentSpan.addClass("selected-segment");
                }, function() {
                    $segmentSpan.removeClass("selected-segment");
                });

                function removeSegment(ci, path) {
                    $segmentSpan.remove();
                    $break.remove();
                    ci.prev.remove();
                    ci.remove();
                    path.remove();
                    count1 -= 2;
                }
                // update double click triggers
                // trigger deletion of entire segment when double clicking anywhere on it
                ci.dblclick(function() { 
                    removeSegment(this, path); 
                });
                ci.prev.dblclick(function(event) {
                    removeSegment(ci, path);
                });
                path.dblclick(function(event) {
                    removeSegment(ci, this);
                });
            } else {
                // double click triggers removal of single point
                ci.dblclick(function() {
                    // don't execute this if the point is already part of a line segment
                    if (this.next === null) {
                        this.remove();
                        count1 -= 1;
                    }
                });
            }
            lastx1 = posx;
            lasty1 = posy;
            count1 = count1+1;
        }

        function draw2(e) {
            var pos = getPos(e);
            var posx = pos[0];
            var posy = pos[1];
            //segment start = circle, end = rectangle
            if (count2%2>0) {
              //var ci=paper.rect(posx-4,posy-4,8,8).attr({fill: getSelectedColorValueVariable(count2 - 1)});
              var ci=paper.rect(posx-4,posy-4,8,8).attr({fill: getSelectedColorValue()});
            } else {
              //var ci=paper.circle(posx,posy, 5).attr({fill: getSelectedColorValueVariable(count2)});
              var ci=paper.circle(posx,posy, 5).attr({fill: getSelectedColorValue()});
            }
            if (count2%2>0){
                //overwrite the end-point for segment end
                //var path = paper.path("M"+lastx2+" "+lasty2+" L"+posx+" "+posy)
                  //.attr({stroke: getSelectedColorValueVariable(count2 - 1)});
                var path = paper.path("M"+lastx2+" "+lasty2+" L"+posx+" "+posy)
                  .attr({stroke: getSelectedColorValue()});
                var $segmentSpan =  writeCppSegment("segsAfter", lastx2-cornerx2, lasty2-cornery, posx-cornerx2, posy-cornery);
                var $break = $("<br>");
                $("#seg2").append($segmentSpan).append($break);

                // highlight corresponding line of code on hover of the segment
                ci.hover(function() {
                    $segmentSpan.addClass("selected-segment");
                }, function() {
                    $segmentSpan.removeClass("selected-segment");
                });
                ci.prev.hover(function() {
                    $segmentSpan.addClass("selected-segment");
                }, function() {
                    $segmentSpan.removeClass("selected-segment");
                });
                path.hover(function() {
                    $segmentSpan.addClass("selected-segment");
                }, function() {
                    $segmentSpan.removeClass("selected-segment");
                });

                function removeSegment(ci, path) {
                    $segmentSpan.remove();
                    $break.remove();
                    ci.prev.remove();
                    ci.remove();
                    path.remove();
                    count2 -= 2;
                }

                // update double click triggers
                // trigger deletion of entire segment when double clicking anywhere on it
                ci.dblclick(function() { 
                    removeSegment(this, path); 
                });
                ci.prev.dblclick(function(event) {
                    removeSegment(ci, path);
                });
                path.dblclick(function(event) {
                    removeSegment(ci, this);
                });
            } else {
                // double click triggers removal of single point
                ci.dblclick(function() {
                    // don't execute this if the point is already part of a line segment
                    if (this.next === null) {
                        this.remove();
                        count2 -= 1;
                    }
                });
            }
            lastx2=posx;
            lasty2=posy;
            count2=count2+1;
        }

        var bfg=paper.image(path1, cornerx1, cornery, imagewidth, imageheight);
        bfg.click(draw1);
        var bfg2=paper.image(path2, cornerx2, cornery, imagewidth, imageheight);
        bfg2.click(draw2);
    };
});
