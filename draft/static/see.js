
function parseData(data){

    var parsed = JSON.parse(data);

    return (parsed.data)


}
var vehicle_status = new EventSource('/isAlive')

  vehicle_status.onmessage = function(message){
  $('#infoBanner_data').text(message.data);
  console.log(message.data);

  };

var fetch_logs = new EventSource('/getLogStream')

  fetch_logs.onmessage = function(message){
    $('#cc_panel').append(parseData(message.data));
  console.log(message.data);

  };


  function makeSVG(tag, attrs) {
    var el= document.createElementNS('http://www.w3.org/2000/svg', tag);
    for (var k in attrs)
      el.setAttribute(k, attrs[k]);
      console.log(el);
    return el;
  }

  var pathTest = [
  [-10,0,"wall"],[10,0,"wall"],[-10,10,"wall"],[10,10,"wall"],
  [0,20,"wall"],[-10,20,"wall"],[-10,30,"wall"],[0,30,"wall"],
  [-5,50,"wall"],[0,40,"wall"],[-10,50,"wall"],[-10,30,"wall"],
  [-20,50,"wall"],[-20,30,"wall"],[-30,50,"wall"],[-30,30,"wall"],
  [-40,40,"wall"]];

  var svgPath = "M"+pathTest[0][0]+' '+pathTest[0][1]+' ';
  var marker = "";
  $(document).ready(
  function getCoord()
  {
    for (var i=1;i<pathTest.length;i++)
    {
        svgPath += "L"+pathTest[i][0]+' '+pathTest[i][1]+' ';
        marker = makeSVG('circle', {cx: pathTest[i][0], cy: pathTest[i][1], r:1, stroke: 'black', 'stroke-width': .5, fill: 'red'});
        $("#testViz").append(marker);
    }
    console.log(svgPath);

  });

$(document).ready(
        function(){
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
