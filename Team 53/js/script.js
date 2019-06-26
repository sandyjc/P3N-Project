var descriptionBox = document.getElementById("description");
descriptionBox.innerHTML = "three.js 3D cube using BufferGeometry lines";

//Setup renderer
var renderer = new THREE.WebGLRenderer();
renderer.setSize( window.innerWidth, window.innerHeight );
document.body.appendChild( renderer.domElement );

//This will include all rendered objects
var image = new THREE.Object3D();

//Setup camera
var camera = new THREE.PerspectiveCamera( 45, window.innerWidth / window.innerHeight, 1, 500 );
camera.position.set(0,0,10);
//This allows for rotation of camera
var controls = new THREE.OrbitControls(camera);
controls.update();

var scene = new THREE.Scene();

//Setup cursor
var cursorgeometry = new THREE.SphereGeometry( .05, 32, 32 );
var cursormaterial = new THREE.MeshBasicMaterial( {color: 0xffff00} );
var cursor = new THREE.Mesh( cursorgeometry, cursormaterial );
image.add( cursor );

//Setup grid floor
var size = 1000;
var divisions = 1000;
var gridHelper = new THREE.GridHelper( size, divisions );
image.add( gridHelper );


//Setup new line object
var LINEWIDTH = 100;
var LINECOLOR = 0x0000ff;
var DRAWCOUNT = 0;
var currentLine = makeNewLine();
image.add( currentLine );



//Rotate all objects to isometric view
var angle = .1;
image.rotation.set(5*angle, 10*angle,0);

scene.add( image );
animate();

var api_index = -1;
doDrawingApi();
doCommandsApi();
updateDrawing();


function makeNewLine() {
    var MAX_POINTS = 10000;
    var drawgeometry = new THREE.BufferGeometry();
    // attributes
    var drawpositions = new Float32Array( MAX_POINTS * 3 ); // 3 vertices per point
    drawgeometry.addAttribute( 'position', new THREE.BufferAttribute( drawpositions, 3 ) );
    // drawcalls
    DRAWCOUNT = 0; // draw the first 2 points, only
    drawgeometry.setDrawRange( 0, DRAWCOUNT );
    // material
    var drawmaterial = new THREE.LineBasicMaterial({color: LINECOLOR, linewidth: LINEWIDTH});
    // line
    drawline = new THREE.Line( drawgeometry,  drawmaterial );
    
    return drawline;
}

//Do api call every second
function doDrawingApi() {
    var request;
    setInterval(function() {
        request = new XMLHttpRequest();
        request.open('GET', 'http://608dev.net/sandbox/sc/ykinsew/project/608_server.py?user=yoda&type=position', true);
        request.onload = function () {
            if (request.status == "200") {
                var data = JSON.parse(request.response.replace(/'/g, '"'));
                if (data['points'].length > 0) {
                    for (var i = 0; i < data['points'].length; i++) {
                        if (data['states'][i] == "draw") {
                            newx = data['points'][i][0];
                            newy = data['points'][i][1];
                            newz = data['points'][i][2];
                            if (api_index >= 3) {
                                oldx = currentLine.geometry.attributes.position.array[api_index-2]
                                oldy = currentLine.geometry.attributes.position.array[api_index-1]
                                oldz = currentLine.geometry.attributes.position.array[api_index]
                                dx = (newx - oldx)/10
                                dy = (newy - oldy)/10
                                dz = (newz - oldz)/10
                                x = oldx; y = oldy; z = oldz;
                                for (var j = 0; j <= 10; j++) {
                                    api_index += 1;
                                    currentLine.geometry.attributes.position.array[api_index] = x;
                                    api_index += 1;
                                    currentLine.geometry.attributes.position.array[api_index] = y;
                                    api_index += 1;
                                    currentLine.geometry.attributes.position.array[api_index] = z;
                                    x += dx;
                                    y += dy;
                                    z += dz;
                                }
                            } else {
                                api_index += 1;
                                currentLine.geometry.attributes.position.array[api_index] = newx;
                                api_index += 1;
                                currentLine.geometry.attributes.position.array[api_index] = newy;
                                api_index += 1;
                                currentLine.geometry.attributes.position.array[api_index] = newz;
                            }
                        }
                        if (data['states'][i] == "stop") {
                            currentLine.material.color.setHSL( Math.random(), 1, 0.5 );
                            currentLine = makeNewLine();
                            api_index = -1;
                            DRAWCOUNT = 0;
                            image.add( currentLine );
                        }
                    }
                }
            }
        }
        request.send();
    },1000);
}
// Every .1 seoncds draw a new point from the positions
// if there are no more points, then wait
function updateDrawing() {
    setInterval(function() {
        if (DRAWCOUNT < (api_index)/3) {
            currentLine.geometry.attributes.position.needsUpdate = true;
            currentLine.geometry.setDrawRange(0,DRAWCOUNT);
            cursor.position.x = currentLine.geometry.attributes.position.array[DRAWCOUNT*3-3]
            cursor.position.y = currentLine.geometry.attributes.position.array[DRAWCOUNT*3-2]
            cursor.position.z = currentLine.geometry.attributes.position.array[DRAWCOUNT*3-1]
            render();
            DRAWCOUNT += 1;
        }
    }, 20);
}


function animate () {
    requestAnimationFrame( animate );
    // controls.autoRotate = false;
    // controls.update();
    render();
}

function render () {
    renderer.render(scene, camera);
}


function updateLine() {
    currentLine.material.linewidth = LINEWIDTH;
    currentLine.material.color = new THREE.Color(LINECOLOR);
}

function doCommandsApi() {
    var request;
    setInterval(function() {
        request = new XMLHttpRequest();
        request.open('GET', 'http://608dev.net/sandbox/sc/ykinsew/project/608_server.py?user=yoda&type=command', true);
        request.onload = function() {
            if (request.status == "200") {
                var data = JSON.parse(request.response.replace(/'/g, '"'));
                if (data["user"] != "") {
                    LINEWIDTH = data["line_width"];
                    LINECOLOR = data["color"];
                    updateLine();
                }
                // if (data["command"] != "") {
                //     console.log(data["command"]);
                //     switch(data["command"]) {
                //         case "help":
                //             descriptionBox.innerHTML = "you need help";
                //             break;
                //         case "look":
                //             descriptionBox.innerHTML = "changing camera to look at [0,0,0]";
                //             break;
                //         case "rotate":
                //             descriptionBox.innerHTML = "rotating camera 20% in positive y direction";
                //         case "submit":
                //             descriptionBox.innerHTML = "submitting 3D drawing";
                //         case "stats":
                //             descriptionBox.innerHTML = "x position: 0\ny position: 0\nz position: 0"
                //     }
                // }
            }
        }
        request.send();
    },1000);
}