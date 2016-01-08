var my_API = 'MDE4MDY0NTg0MDE0MjIzMTk3ODFlOGVhMA001';

// 1001 = News
// 1002 = Home Page News All
// API call definitions
var story_id_list = ["1001"];
var num_results = 5;
var output_type = 'JSON';
var fields = ['title', 'teaser', 'text'];
var required_assets = 'text';

// temp API call holders
var story_id_string = '';
var story_fields = '';

// temp var holders
var story_titles = [];
var story_teasers = [];
var story_texts = [];

// mod API call to append arrays
if (story_id_list.length > 1) {
  for (var i = 0; i < story_id_list.length; i++) {
      story_id_string += [i] + ",";
  }

} else {
  story_id_string += story_id_list[0];
}

if (fields.length > 1) {
  for (var i = 0; i < fields.length; i++) {
      story_fields += [i] + ",";
  }

} else {
  fields += story_fields[0];
}

// Create URL
var url_query = 'http://api.npr.org/query?id=' + story_id_string + '&fields=' +
                fields + '&requiredAssets=' + required_assets + '&output=' +
                output_type + '&num_results=' + num_results.toString() +
                '&apiKey=' + my_API;

fetchNews(); // Get NPR API call
// for(var m = 0; m < story_texts.length; m++){
//   console.log('STORY:' + m);
//   console.log(story_titles[m]);
//   console.log(story_teasers[m]);
//   console.log(story_texts[m]);
// }
sendPebble(); // Store variables in Pebble json file

function fetchNews(){
  var xmlhttp = new XMLHttpRequest();
  xmlhttp.onreadystatechange = function() {
    if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
      var response = JSON.parse(xmlhttp.responseText);

      for (var i = 0; i < num_results; i++){

        var cur_story_text = "";
        var cur_story_length = (response.list.story[i].text.paragraph).length;

        for (var j = 0; j < cur_story_length; j++) {

          cur_text_obj = response.list.story[i].text.paragraph[j].$text;
          if (cur_text_obj != null){
            cur_story_text = cur_story_text + cur_text_obj  + '\n\n';
          }
        }
        story_titles[i] = response.list.story[i].title.$text;
        story_teasers[i] = response.list.story[i].teaser.$text;
        story_texts[i] = cur_story_text;
      }
    }
  };
  xmlhttp.open("GET", url_query, false);
  xmlhttp.send(null);
}

function sendPebble(){
  Pebble.sendAppMessage({
    'STORY_COUNT': num_results,
    'STORY_TITLES': story_titles,
    'STORY-TEASERS': story_teasers,
    'STORY_TEXTS': story_texts
  });
}
