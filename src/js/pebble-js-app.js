var my_API = 'MDE4MDY0NTg0MDE0MjIzMTk3ODFlOGVhMA001';

// API call definitions. 1001 = News. 1002 = Home Page News All
var story_id_list = ["1002"];
var num_results = 11;
var act_num_results = 0; // Initalize to 1, increases as data is successfully accessed

var include_text = false; // Parse text or not

function createQuery(num_results) {

  var output_type = 'JSON';
  var fields = ['title', 'teaser', 'text'];
  var required_assets = 'text';

  // temp API call holders
  var story_id_string = '';
  var story_fields = '';

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
    output_type + '&numResults=' + num_results.toString() +
    '&apiKey=' + my_API;

  // console.log(url_query); // Log the url Query
  return url_query;
}

function fetchNews() {
  var url_query = createQuery(num_results); // Creates an instance of the query

  // temp var holders
  var story_titles = "";
  var story_teasers = "";
  var story_texts = "";

  var xmlhttp = new XMLHttpRequest();
  xmlhttp.onreadystatechange = function() {
    if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
      var response = JSON.parse(xmlhttp.responseText);

      for (var i = 0; i < num_results; i++) {

        if (include_text == true) {
          try {
            var cur_story_text = "";
            var cur_story_length = (response.list.story[i].text.paragraph).length;

            for (var j = 0; j < cur_story_length; j++) {

              cur_text_obj = response.list.story[i].text.paragraph[j].$text;
              if (cur_text_obj != null) {
                cur_story_text = cur_story_text + cur_text_obj + '\n\n';
              }
            }

            story_texts = story_texts.concat(cur_story_text);
            story_texts = story_texts.concat("|");
          } catch (err) {
            console.log("response.list.story[" + i + "] is undefined");
          }
        }

        // story_titles[i] = response.list.story[i].title.$text;
        try {
          story_titles = story_titles.concat(response.list.story[i].title.$text);
          story_titles = story_titles.concat("|");
          act_num_results++; // Increment if story title successfully added
        } catch (err) {
          console.log("response.list.story[" + i + "] is undefined");
        }

        try {
          story_teasers = story_teasers.concat(response.list.story[i].teaser.$text);
          story_teasers = story_teasers.concat("|");
        } catch (err) {
          console.log("response.list.story[" + i + "] is undefined");
        }
        // story_texts = story_texts.concat(response.list.story[i].teaser.$text);
        // story_texts = story_texts.concat("|");

        // This appends the null character onto the c string
        if (i == (num_results - 1)) {
          story_titles = story_titles.concat("\0");
          story_teasers = story_teasers.concat("\0");
          story_texts = story_texts.concat("\0");
        }
      }
    }
  };
  xmlhttp.open("GET", url_query, false);
  xmlhttp.send(null);

  var dict_info = {
    'story_count': act_num_results,
    'story_titles': story_titles,
    'story_teasers': story_teasers
  };

  var dict_texts = {
    'story_texts': story_texts
  };

  var dict = [dict_info, dict_texts];
  return dict;
}

Pebble.addEventListener('ready', function() {
  // PebbleKit JS is ready!
  console.log('PebbleKit JS ready!');

  // Get the results from fetchNews
  dict = fetchNews();

  // This is just to test
  dict_size_0 = roughSizeOfObject(dict[0]);
  dict_size_1 = roughSizeOfObject(dict[1]);

  // Send the object
  Pebble.sendAppMessage(dict[0], function() {
    console.log('Message sent successfully');
    // console.log(JSON.stringify(dict[0]));
  }, function(e) {
    console.log('Message failed: ' + JSON.stringify(e));
  });

});

function roughSizeOfObject(object) {

  var objectList = [];
  var stack = [object];
  var bytes = 0;

  while (stack.length) {
    var value = stack.pop();

    if (typeof value === 'boolean') {
      bytes += 4;
    } else if (typeof value === 'string') {
      bytes += value.length * 2;
    } else if (typeof value === 'number') {
      bytes += 8;
    } else if (
      typeof value === 'object' && objectList.indexOf(value) === -1
    ) {
      objectList.push(value);

      for (var i in value) {
        stack.push(value[i]);
      }
    }
  }
  return bytes;
}
