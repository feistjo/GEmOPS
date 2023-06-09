var testText = document.getElementById("test");
var step_point = 10;
//var img_URL;
var display_img = document.getElementById("user_img");
paper = Raphael(document.getElementById("canvas"), "50%", "50%");
current_displayed_paths = null;

var gateway = `ws://192.168.4.1/ws`;
var websocket;

// ----------------------------------------------------------------------------
// Websocket
// ----------------------------------------------------------------------------

window.addEventListener("load", onLoad);

function onLoad(event) {
  initWebSocket();
}

function initWebSocket() {
  console.log("Trying to open a WebSocket connection...");
  websocket = new WebSocket(gateway);
  websocket.onopen = function () {
    alert("Connection opened");
  };

  websocket.onclose = function () {
    alert("Connection closed");
  };
  websocket.onmessage = onMessage;
}

function onMessage(event) {
  console.log(`Received a notification from ${event.origin}`);
  console.log(event);
}

function getInfosFromPaths(paths) {
  var paths_info = [];
  var initialized = false;
  for (var i = 0; i < paths.length; ++i) {
    console.log(paths.item(i));
    var path = $($(paths).get(i)).attr("d").replace(" ", ",");
    var shape = paper.path(path);
    var bbox_path = shape.getBBox();
    shape.remove();

    if (!initialized) {
      initialized = true;
      paths_info.bbox_top =
        paths_info.bbox_bottom =
        paths_info.bbox_left =
        paths_info.bbox_right =
          bbox_path;
      continue;
    }

    if (paths_info.bbox_top != bbox_path && paths_info.bbox_top.y > bbox_path.y)
      paths_info.bbox_top = bbox_path;
    if (
      paths_info.bbox_bottom != bbox_path &&
      bbox_path.y + bbox_path.height >
        paths_info.bbox_bottom.y + paths_info.bbox_bottom.height
    )
      paths_info.bbox_bottom = bbox_path;
    if (
      paths_info.bbox_left != bbox_path &&
      paths_info.bbox_left.x > bbox_path.x
    )
      paths_info.bbox_left = bbox_path;
    if (
      paths_info.bbox_right != bbox_path &&
      bbox_path.x + bbox_path.width >
        paths_info.bbox_right.x + paths_info.bbox_right.width
    )
      paths_info.bbox_right = bbox_path;
  }

  paths_info.width =
    paths_info.bbox_right.x +
    paths_info.bbox_right.width -
    paths_info.bbox_left.x;
  paths_info.height =
    paths_info.bbox_bottom.y +
    paths_info.bbox_bottom.height -
    paths_info.bbox_top.y;
  paths_info.x = paths_info.bbox_left.x;
  paths_info.y = paths_info.bbox_top.y;
  if (paths_info.height > paths_info.width)
    paths_info.scale =
      paths_info.height > paper.canvas.clientHeight
        ? paper.canvas.clientHeight / paths_info.height
        : 1;
  else
    paths_info.scale =
      paths_info.width > paper.canvas.clientWidth
        ? paper.canvas.clientWidth / paths_info.width
        : 1;

  // console.log(paths_info);
  // Display bboxes used for centering paths
  // var bboxes = [paths_info.bbox_right, paths_info.bbox_left, paths_info.bbox_top, paths_info.bbox_bottom];
  // for (var i = 0; i < 4; ++i) {
  //     var container = paper.rect(bboxes[i].x + 300, bboxes[i].y + 300, bboxes[i].width, bboxes[i].height);
  //     container.attr("stroke", "red");
  // }

  return paths_info;
}

function convertSVG(current_svg_xml) {
  //console.log(current_svg_xml);

  paper.clear();
  var parser = new DOMParser();
  var doc = parser.parseFromString(current_svg_xml, "application/xml");
  var paths = doc.getElementsByTagName("path");
  console.log(paths);
  current_displayed_paths = paths;

  // Read each paths from svg
  var paths_info = getInfosFromPaths(paths);
  var offset_path_x =
    paths_info.x * paths_info.scale * -1 +
    paper.canvas.clientWidth / 2 -
    (paths_info.width * paths_info.scale) / 2;
  var offset_path_y =
    paths_info.y * paths_info.scale * -1 +
    paper.canvas.clientHeight / 2 -
    (paths_info.height * paths_info.scale) / 2;
  var all_points = {};
  var all_points_count = 0;
  for (var i = 0; i < paths.length; ++i) {
    var path = $($(paths).get(i)).attr("d").replace(" ", ",");

    // get points at regular intervals
    //var data_points = "";
    var color = "red";
    var j;

    for (j = 0; j * step_point < Raphael.getTotalLength(path); j++) {
      var point = Raphael.getPointAtLength(path, j * step_point);

      all_points[j + i] = { x: point.x, y: point.y }; // TODO: this should work now 6/6

      var circle = paper
        .circle(point.x * paths_info.scale, point.y * paths_info.scale, 2)
        .attr("fill", color)
        .attr("stroke", "none")
        .transform(
          "T" +
            offset_path_x * paths_info.scale +
            "," +
            offset_path_y * paths_info.scale
        );
    }

    all_points_count += j;
    //all_points += data_points + "#&#13;";
    //        addBelow("Path " + i, color, data_points, c / step_point);
  }

  all_points["num_points"] = Object.keys(all_points).length; // all_points_count;

  // Send that shit over!
  console.log(JSON.stringify(all_points));
  websocket.send(JSON.stringify(all_points));

  //    addBelow("All Paths", "#2A2A2A", all_points, all_points_count / step_point);

  //    $('.bellows').bellows();
  //    hideHoldOnOverlay();
}

function readSVG(event) {
  const fileList = selectedFile.files;
  var user_img = fileList[0];
  const reader = new FileReader();
  reader.addEventListener("load", (event) => {
    var current_svg_xml = event.target.result;
    convertSVG(current_svg_xml);
  });
  reader.addEventListener("progress", (event) => {
    if (event.loaded && event.total) {
      const percent = (event.loaded / event.total) * 100;
      console.log(`Progress: ${Math.round(percent)}`);
    }
  });
  reader.readAsBinaryString(user_img);
}
var selectedFile = document.getElementById("my_file");

selectedFile.addEventListener("change", readSVG, false);

//document.getElementById("file").addEventListener("submit", (event) =>{
//
//    //points exported from DrawScript : Bezier Points Array (anchor, control, control, anchor)
//    var data = Snap.path.toCubic(document.getElementById("path").getAttribute('d'));
//    dataLength = data.length();
//    points = []; //holds our series of x/y values for anchors and control points,
//    pointsString = data.toString();
//
//    // convert cubic data to GSAP bezier
//    for (var i = 0; i < dataLength; i++) {
//      var seg = data[i];
//      if (seg[0] === "M") { // move (starts the path)
//        var point = {};
//        point.x = seg[1];
//        point.y = seg[2];
//        points.push(point);
//      } else { // seg[0] === "C" (Snap.path.toCubic should return only curves after first point)
//        for (var j = 1; j < 6; j+=2) {
//          var point = {};
//          point.x = seg[j];
//          point.y = seg[j+1];
//          points.push(point);
//        }
//      }
//    }
//})
