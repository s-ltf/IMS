/*Controller.js*/
/* Helper Functions */
function parseData(data){
    var parsed = JSON.parse(data);
       return parsed
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
            level = null;
            tag = null;
      }else{
            parsedData = parseData(message.data);
            msg = parsedData.data;
            level = parsedData.level;
            tag = parsedData.tag;
      }

      if(level == 'info'){
          color = 'blue';
      }
      else if(level == 'error'){
          color = 'blue';
      }
      else if(level == 'debug'){
          color = 'blue';
      }
      else if(level == 'warning'){
          color = 'blue';
      }
      else{
          color = '#1E8F0E';
      }

      if(tag == 'shared'){
          tag_target= '#shared_mem_output';
          tag_container= '#shared_mem_panel';
      }
      else if (tag == 'serial'){
          tag_target= '#serial_output';
          tag_container= '#serial_panel';
      }
      else if (tag == 'mmu'){
          tag_target= '#mmu_output';
          tag_container= '#mmu_panel';
      }
      else if (tag == 'poi'){
          tag_target= '#poi_output';
          tag_container= '#poi_panel';
      }
      else{
          tag_target= '#a_panel';
          tag_container= '#a_panel';
      }
    $(tag_target).append('<p style="color:'+color+';margin:0">'+msg+"</p><br>");
    $(tag_container).scrollTop($(tag_target).height());

    $(live_feed_obj.target).append('<p style="color:'+color+';margin:0">'+msg+"</p><br>");
    $(live_feed_obj.target_container).scrollTop($(live_feed_obj.target).height());

  };


function feed_object( name , target , url , target_container){
    target_container = target_container || "''";
    this.name = name;
    this.target = target;
    this.url = url;
    this.target_container = target_container;
};




function resetPanels(){
    $(".output").empty();

    $("#viewport").empty();
    $("#viewport").append(makeSVG('line',{ x1:"-200", y1:"0",x2:"200",y2:"0",stroke:"black"}));
    $("#viewport").append(makeSVG('line',{ x1:"0", y1:"-200",x2:"0",y2:"200",stroke:"black"}));
    $("#viewport").append(makeSVG('g',{ id: "vehicle"}));
    alert('All Panels Cleared');

};

/* ------------- */

var live_feed_objects ={
    "vehicle_status": new feed_object("vehicle_status","#infoBanner_data","/isAlive"),
    "viz_feed": new feed_object("viz_feed","#output","/getVizFeed"),
    "log_feed": new feed_object("log_feed","#all_output","/getLogStream", "#all_panel") ,
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



eventSource_objects["log_feed"].onmessage = function(message){
    var temp = live_feed_objects["log_feed"];
    console.log(message);
    processFeed(message,temp);
};

$(document).ready(
        function(){
            svgPanZoom.init();
            $('#reset').click(function(){
                resetPanels();
            });
            $('#save').click(function(){
                alert('FUNCTIONALITY NOT IMPLEMENTED YET'); //Saved Generated Image - 
            });
            $('#stop').click(function(){
                alert('FUNCTIONALITY NOT IMPLEMENTED YET'); 
                /*var r = confirm('Send Kill Signal to Robot');
                if (r == true){
                    alert('Kill Signal Sent');
                }*/
            });

        });
