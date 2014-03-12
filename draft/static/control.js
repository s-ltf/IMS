/* Helper Functions */
function parseData(data){
    var parsed = JSON.parse(data);
       return (parsed.data)
}

function parseJSON (data){
    return JSON.parse(data);
}

function makeSVG(tag, attrs) {
    var el= document.createElementNS('http://www.w3.org/2000/svg', tag);
    for (var k in attrs)
      el.setAttribute(k, attrs[k]);
      console.log(el);
    return el;
}

function svg_marker(){
    return makeSVG('circle', {cx:coord_x , cy:coord_y, r:1, stroke: 'black', 'stroke-width': .5, fill: 'red'});
}

function processFeed_status(message,live_feed_obj){

    $(live_feed_obj.target).text(message.data);
    console.log(message.data);

}


function processFeed (message,live_feed_obj){
      if (message.data == 1){
            msg = 'server online - '+live_feed_obj.name;
      }else{
            msg = parseData(message.data);
      }
    $(live_feed_obj.target).append(msg+"<br>");
    $(live_feed_obj.target_container).scrollTop($(live_feed_obj.target).height());
  console.log(message.data);

  };


function feed_object( name , target , url , target_container){
    target_container = target_container || "''";
    this.name = name;
    this.target = target;
    this.url = url;
    this.target_container = target_container;
};


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
/* ------------- */

var live_feed_objects ={
    "vehicle_status": new feed_object("vehicle_status","#infoBanner_data","/isAlive"),
    "viz_feed": new feed_object("viz_feed","#output","/getVizFeed"),
    "serial_feed" : new feed_object("serial_feed","#output","/getSerialStream","#cc_panel"),
    "log_feed": new feed_object("log_feed","#output","/getLogStream", "#cc_panel") ,
    };


var eventSource_objects = {};
for (var key in live_feed_objects){
    eventSource_objects[key] = new EventSource(live_feed_objects[key].url);
};



eventSource_objects["vehicle_status"].onmessage = function(message){
    var temp = live_feed_objects["vehicle_status"];
    processFeed_status(message,temp);
    };

eventSource_objects["serial_feed"].onmessage = function(message){
    var temp = live_feed_objects["serial_feed"];
    processFeed(message,temp);
};


eventSource_objects["log_feed"].onmessage = function(message){
    var temp = live_feed_objects["log_feed"];
    processFeed(message,temp);
};


var marker ="";
var coord_x,coord_y ="";
eventSource_objects["viz_feed"].onmessage = function(message){
        if(message.data == 1){
            $('#output').append("server online - Visualizer Stream"+"<br>");
        }else{
        //console.log(message.data);
        coord_x = coord(message.data,'x');
        coord_y = coord(message.data,'y');
        coord_x = coord_x/5.0;
        coord_y = -1*coord_y/5.0;
        console.log(coord_x + ' '+ coord_y);
        marker =  svg_marker();
        $('#viewport').append(marker);
        $('#output').append("x: "+coord_x+"\ty: "+coord_y+"<br>");
        }
        $('#cc_panel').scrollTop($('#output').height());

};

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
