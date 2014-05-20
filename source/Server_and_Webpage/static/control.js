/* Helper Functions */
function parseData(data){
    var parsed = JSON.parse(data);
       return (parsed.data)
}

function parseJSON (data){
    return JSON.parse(data);
}



function processFeed_status(message,live_feed_obj){

    $(live_feed_obj.target).text(message.data);
    //console.log(message.data);

}


function processFeed (message,live_feed_obj){
      if (message.data == 1){
            msg = 'server online - '+live_feed_obj.name;
      }else{
            msg = parseData(message.data);
      }
    $(live_feed_obj.target).append(msg+"<br>");
    $(live_feed_obj.target_container).scrollTop($(live_feed_obj.target).height());
  //console.log(message.data);

  };


function feed_object( name , target , url , target_container){
    target_container = target_container || "''";
    this.name = name;
    this.target = target;
    this.url = url;
    this.target_container = target_container;
};




function resetPanels(){
    $("#output").text("Reset");
    $("#viewport").empty();
    $("#viewport").append(makeSVG('line',{ x1:"-200", y1:"0",x2:"200",y2:"0",stroke:"black"}));
    $("#viewport").append(makeSVG('line',{ x1:"0", y1:"-200",x2:"0",y2:"200",stroke:"black"}));
    $("#viewport").append(makeSVG('g',{ id: "vehicle"}));
}

/* ------------- */

var live_feed_objects ={
    "vehicle_status": new feed_object("vehicle_status","#infoBanner_data","/isAlive"),
    "viz_feed": new feed_object("viz_feed","#output","/getVizFeed"),
    "serial_feed" : new feed_object("serial_feed","#output","/getSerialStream","#cc_panel"),
    "log_feed": new feed_object("log_feed","#output","/getLogStream", "#cc_panel") ,
    "pose_feed": new feed_object("pose_feed","#vehicle","/getPoseFeed"),
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
    console.log(message);
    processFeed(message,temp);
};


$(document).ready(
        function(){
            resetPanels();
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

            $('#reset_id').click(function(){
                resetPanels();
            });


        });
