
function parseData(data){

    var parsed = JSON.parse(data);
       return (parsed.data)


}

function makeSVG(tag, attrs) {
    var el= document.createElementNS('http://www.w3.org/2000/svg', tag);
    for (var k in attrs)
      el.setAttribute(k, attrs[k]);
      console.log(el);
    return el;
  }

function parseJSON (data){
    return JSON.parse(data);
}
function coord(data,axis){
    //console.log("iside ParseData -1 " + data);
    var parsed = parseJSON(data);
    //console.log("inside ParseData "+ parsed.data);
    var parsed2 = parseJSON(parsed.data);
    //console.log("inside ParseData2" + parsed2.x);


    if (axis == 'x')
        return parsed2.x;
    else if (axis == 'y')
        return parsed2.y;
    else
        return 0;

}
var vehicle_status = new EventSource('/isAlive')

  vehicle_status.onmessage = function(message){
  $('#infoBanner_data').text(message.data);
  console.log(message.data);

  };

var vizualizer_feed = new EventSource('/getVizFeed')
var marker ="";
var coord_x,coord_y ="";
vizualizer_feed.onmessage = function(message){
        //console.log(message.data);
        coord_x = coord(message.data,'x');
        coord_y = coord(message.data,'y');
        coord_x = coord_x/5.0;
        coord_y = -1*coord_y/5.0;
        console.log(coord_x + ' '+ coord_y);
        marker = makeSVG('circle', {cx:coord_x , cy:coord_y, r:1, stroke: 'black', 'stroke-width': .5, fill: 'red'});
        $('#viewport').append(marker);
$('#output').append("x: "+coord_x+" y: "+coord_y+"<br>");
    $('#cc_panel').scrollTop($('#output').height());

};

var fetch_serial = new EventSource('/getSerialStream');

  fetch_serial.onmessage = function(message){
    $('#output').append(parseData(message.data)+"<br>");
    $('#cc_panel').scrollTop($('#output').height());
  console.log(message.data);

  };

var fetch_logs = new EventSource('/getLogStream')

  fetch_logs.onmessage = function(message){
    $('#output').append(parseData(message.data)+"<br>");
    $('#cc_panel').scrollTop($('#output').height());
  console.log(message.data);

  };

/*
  var pathTest = [
  [-10,0,"wall"],[10,0,"wall"],[-10,10,"wall"],[10,10,"wall"],
  [0,20,"wall"],[-10,20,"wall"],[-10,30,"wall"],[0,30,"wall"],
  [-5,50,"wall"],[0,40,"wall"],[-10,50,"wall"],[-10,30,"wall"],
  [-20,50,"wall"],[-20,30,"wall"],[-30,50,"wall"],[-30,30,"wall"],
  [-40,40,"wall"]];

  var svgPath = "M"+pathTest[0][0]+' '+pathTest[0][1]+' ';
  marker = "";
  $(document).ready(
  function getCoord()
  {
    for (var i=1;i<pathTest.length;i++)
    {
        svgPath += "L"+pathTest[i][0]+' '+pathTest[i][1]+' ';
        marker = makeSVG('circle', {cx: pathTest[i][0], cy: pathTest[i][1], r:1, stroke: 'black', 'stroke-width': .5, fill: 'red'});
        $("#viewport").append(marker);
    }
    console.log(svgPath);

  });
*/
$(document).ready(
        function(){
            svgPanZoom.init();
            $('#svg_panel').hide();
            $('#visualizer_id').click(function(){
                $('#svg_panel').show();
                $('#cc_panel').hide();}
                );

            $('#command_center_id').click(function(){
                $('#svg_panel').hide();
                $('#cc_panel').show();
            });
        });
