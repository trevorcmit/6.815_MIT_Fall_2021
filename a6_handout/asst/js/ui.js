function getString() {
  var S = "CorrespondencePair corresp[4] = {<br />";
  var maxpointsN = Math.max(pointsN[0], pointsN[1]);

  for (var i = 0; i < maxpointsN; i++) {
    S = S + "CorrespondencePair( ";
    if (i < pointsN[0]) {
      S += points[0][i];
    }
    S += ", ";
    if (i < pointsN[1]) {
      S += points[1][i];
    }
    S += ")";
    if (i != maxpointsN-1) {
      S += ", <br />";
    }
  }
  S += "<br />};";
  return S;
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
  posx = posx - offset.left;
  posy = posy - offset.top;

  return [posx,posy];
}

$('document').ready( function() {
  var img1 = new Image();
  img1.src = path1;
  imagewidth1  = 0;
  imageheight1 = 0;

  var img2 = new Image();
  img2.src = path2;
  imagewidth2  = 0;
  imageheight2 = 0;

  var ok1 = false;
  var ok2 = false;

  img1.onload = function() {
    imagewidth1  = img1.width;
    imageheight1 = img1.height;
    ok1 = true;
    process();
  };

  img2.onload = function() {
    imagewidth2  = img2.width;
    imageheight2 = img2.height;
    ok2 = true;
    process();
  };

  function process(){
    if(!ok1 || !ok2){
      return;
    }
    space = 10;
    column_width = imagewidth1+space;
    paper = Raphael(document.getElementById("canvas"),
      space+imagewidth1+imagewidth2, Math.max(imageheight1, imageheight2));


    cornerx1=10;
    cornery=10;
    cornerx2= cornerx1+column_width;

    points = [[''], ['']];
    pointsN = [0, 0];
    draws = [];

    offset = ($("#canvas").offset());

    function draw(e, i) {
      var pos = getPos(e);
      var posx = pos[0];
      var posy = pos[1];
      var color = document.getElementById("colorSwatch").style.backgroundColor;
      var pointNumber = pointsN[i]+1;
      var cc = paper.circle(posx,posy, 6.5).attr({fill: color});
      var ci = paper.text(posx, posy, pointNumber).attr({fill: 'white'});
      ci.attr({
        "font-size": 13,
        "font-family": "Arial, Helvetica, sans-serif"
      });

      var cornerx = (i == 0) ? cornerx1 : cornerx2;
      points[i][pointsN[i]] = ''+(posx-cornerx)+","+(posy-cornery)+",1";
      pointsN[i]++;
      document.getElementById("points").innerHTML=getString();
      draws.push([i, cc, ci]);
    }

    var bfg=paper.image(path1, cornerx1, cornery, imagewidth1, imageheight1);
    bfg.click(function(e) {draw(e, 0)});
    var bfg2=paper.image(path2, cornerx2, cornery, imagewidth2, imageheight2);
    bfg2.click(function(e) {draw(e, 1)});
  }

  function undoLastDraw() {
    if (draws.length > 0) {
      var lastDraw = draws.pop();
      var i = lastDraw[0];
      var cc = lastDraw[1];
      var ci = lastDraw[2];
      cc.remove();
      ci.remove();
      points[i].pop();
      pointsN[i]--;
      document.getElementById("points").innerHTML=getString();
    }
  }

  document.addEventListener("keyup", function(e) {
    if (e.keyCode == 90 && e.ctrlKey && draws.length > 0) {
      undoLastDraw();
    }
  });
});
