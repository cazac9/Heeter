var now = new Date();
var timenow = now.getHours() + (now.getMinutes() / 60);
var days = {
    0: 'mon',
    1: 'tue',
    2: 'wed',
    3: 'thu',
    4: 'fri',
    5: 'sat',
    6: 'sun'
};

var daysNums = {
    'mon': 0,
    'tue': 1,
    'wed': 2,
    'thu': 3,
    'fri': 4,
    'sat': 5,
    'sun': 6
};

var currentDay = now.getDay() - 1;
var today = days[currentDay];

//=================================================
// DATA
//=================================================
var visibleFlag = 1;
var unit ="&deg;C";
var doingsave = false;



var heater = {
    power: 2,
	current: 21,
	relay1state: 0,
    isOn: 2, // false
    target: 21,
    schedule: {},
    isOnSchedule: 2 // false
};

var day1 = [{
    s: 0,
    e: 7,
    t: 45,
    p: 2
}, {
    s: 7,
    e: 22,
    t: 20,
    p: 1
}, {
    s: 22,
    e: 24,
    t: 45,
    p: 2
}];

server_get();

heater.schedule['0'] = JSON.parse(JSON.stringify(day1));
heater.schedule['1'] = JSON.parse(JSON.stringify(day1));
heater.schedule['2'] = JSON.parse(JSON.stringify(day1));
heater.schedule['3'] = JSON.parse(JSON.stringify(day1));
heater.schedule['4'] = JSON.parse(JSON.stringify(day1));
heater.schedule['5'] = JSON.parse(JSON.stringify(day1));
heater.schedule['6'] = JSON.parse(JSON.stringify(day1));

// ================================================
// State variables
// ================================================
var editmode = 'move';
$("#mode-move").css("background-color", "#ff9600");
var key = 1;
var day = 0;
var mousedown = 0;
var slider_width = $(".slider").width();
var slider_height = $(".slider").height();
var changed = 0;
		

setInterval(server_get, 5000);
setInterval(updateclock, 1000);

function updateclock() {
    now = new Date();
    timenow = now.getHours() + (now.getMinutes() / 60);
    today = days[currentDay];
   
	
	checkVisibility();

    $("#datetime").html(today.toUpperCase() + " " + format_time(timenow));

    if (heater.isOnSchedule != 1) {
        $(".zone-setpoint").html(heater.target + unit);
        $(".zone-setpoint-power").html(heater.power);
    }
			
    var current_key = 0;
    for (var z in heater.schedule[currentDay]) {
        if (heater.schedule[currentDay][z].s <= timenow && heater.schedule[currentDay][z].e > timenow) {
            if (heater.isOnSchedule == 1) {
                schedule = heater.schedule[currentDay][z];
                $(".zone-setpoint").html(schedule.t + unit);
                $(".zone-setpoint-power").html(schedule.p);
                current_key = z;
            }
        }

    }

    var sx = $(".slider[day=" + today + "]")[0].offsetLeft;
    var y = $(".slider[day=" + today + "]")[0].offsetTop;
    var x1 = sx + slider_width * (timenow / 24.0);
    var x2 = sx + slider_width * (heater.schedule[currentDay][current_key].s / 24.0);

    x2 = sx;
    $("#timemarker").css('top', y + "px");
    $("#timemarker").css('left', x2 + "px");
    $("#timemarker").css('width', (x1 - x2) + "px");

    for (var dayToRefresh in heater.schedule) draw_day_slider(dayToRefresh);

}

function update() {
	$(".zone-temperature").html(heater.current + "&deg;C");
    $("#zone-power").html(heater.power);
	
	if (heater.isOn === 1) {
		$("#toggle").html("ON");
		$("#toggle").css("background-color", "#ff9600");
	} else {
		$("#toggle").html("OFF");
		$("#toggle").css("background-color", "#555");
	}
	
	if (heater.isOnSchedule === 2) {
		$(".heatermode").css("background-color", "#555");
		$("#manual_heater").css("background-color", "#ff9600");
		$("#scheduled_heater").css("background-color", "#555");
	} else {
		$(".heatermode").css("background-color", "#555");
		$("#manual_heater").css("background-color", "#555");
		$("#scheduled_heater").css("background-color", "#ff9600");
	}
}
	
$("#toggle").click(function () {
    heater.isOn = (heater.isOn == 1 || heater.isOn == 0) ? 2 : 1;

    if (heater.isOn == 1) {
        $("#toggle").html("ON");
        $(this).css("background-color", "#ff9600");
    }
    if (heater.isOn === 2) {
        $("#toggle").html("OFF");
        $(this).css("background-color", "#555");
    }

    save();
});

$("#zone-setpoint-dec").click(function () {
    $(".heatermode").css("background-color", "#555");
    $("#manual_heater").css("background-color", "#ff9600");
    heater.isOnSchedule = 2;
    heater.target -= 2;
    if(heater.target < 20)
        heater.target = 80;
    $(".zone-setpoint").html(heater.target + unit);

    save();
});

$("#zone-setpoint-dec-power").click(function () {
    $(".heatermode").css("background-color", "#555");
    $("#manual_heater").css("background-color", "#ff9600");
    heater.isOnSchedule = 2;
    heater.power -= 1;
    if(heater.power < 1)
        heater.power = 3;
    $(".zone-setpoint-power").html(heater.power);

    save();
});

$("#zone-setpoint-inc").click(function () {
    $(".heatermode").css("background-color", "#555");
    $("#manual_heater").css("background-color", "#ff9600");
    heater.isOnSchedule = 2;
    heater.target += 2;
    if(heater.target > 80)
        heater.target = 20;
    $(".zone-setpoint").html(heater.target + unit);

    save();
});

$("#zone-setpoint-inc-power").click(function () {
    $(".heatermode").css("background-color", "#555");
    $("#manual_heater").css("background-color", "#ff9600");
    heater.isOnSchedule = 2;
    heater.power += 1;
    if(heater.power > 3)
        heater.power = 1;
    $(".zone-setpoint-power").html(heater.power);

    save();
});

// ============================================
// SCHEDULER

for (var dayToRefresh in heater.schedule) draw_day_slider(dayToRefresh);

function draw_day_slider(day) {
    var out = "";
    var key = 0;
    for (var z in heater.schedule[day]) {
        var left = (heater.schedule[day][z].s / 24.0) * 100;
        var width = ((heater.schedule[day][z].e - heater.schedule[day][z].s) / 24.0) * 100;
        var color = color_map(heater.schedule[day][z].t);

        out += "<div class='slider-segment' style='left:" + left + "%; width:" + width + "%; background-color:" + color + "' key=" + key + " title='" + heater.schedule[day][z].t + unit +"'></div>";

        if (key > 0) {
            out += "<div class='slider-button' style='left:" + left + "%;' key=" + key + "></div>";
        }
        key++;
    }
    out += "<div class='slider-label'>" + days[day].toUpperCase() + "</div>";
    $(".slider[day=" + days[day] + "]").html(out);
}

$("body").on("mousedown", ".slider-button", function (e) {
    mousedown = 1;
    key = $(this).attr('key');
});

$("body").mouseup(function (e) {
    mousedown = 0;
    if (changed) {
        save();
        changed = 0;
    }
});

$("body").on("mousemove", ".slider", function (e) {
    if (mousedown && editmode == 'move') {
        day = daysNums[$(this).attr('day')];
        slider_update(e);
    }
});

$("body").on("touchstart", ".slider-button", function (e) {
    mousedown = 1;
    key = $(this).attr('key');
});

$("body").on("touchend", ".slider-button", function (e) {
    mousedown = 0;
    if (changed) {
        save();
        changed = 0;
    }
});

$("body").on("touchmove", ".slider", function (e) {

    var event = window.event;
    e.pageX = event.touches[0].pageX;
    if (mousedown && editmode == 'move') {
        day = daysNums[$(this).attr('day')];
        slider_update(e);
    }
});

// MERGE
$("body").on("click", ".slider-button", function () {
    if (editmode == 'merge') {
        day = daysNums[$(this).parent().attr("day")];
        key = parseInt($(this).attr("key"),10);
        heater.schedule[day][key - 1].e = heater.schedule[day][key].e;
        heater.schedule[day].splice(key, 1);
        draw_day_slider(day);
        //editmode = 'move';
        save();
    }
});

$("body").on("click", ".slider-segment", function (e) {

    day = daysNums[$(this).parent().attr("day")];
    key = parseInt($(this).attr("key"),10);

    if (editmode == 'split') {
        var x = e.pageX - $(this).parent()[0].offsetLeft;
        var prc = x / slider_width;
        var hour = prc * 24.0;
        hour = Math.round(hour / 0.5) * 0.5;

        if (hour > heater.schedule[day][key].s + 0.5 && hour < heater.schedule[day][key].e - 0.5) {
            var end = parseFloat(heater.schedule[day][key].e);
            heater.schedule[day][key].e = hour;

            heater.schedule[day].splice(key + 1, 0, {
                s: hour,
                e: end,
                sp: heater.schedule[day][key].t
            });

            draw_day_slider(day);
            save();
        }
        //editmode = 'move';
    } else if (editmode == 'move') {
        $("#slider-segment-temperature").val(heater.schedule[day][key].t);
        $("#slider-segment-power").val(heater.schedule[day][key].p);
        $("#slider-segment-start").val(format_time(heater.schedule[day][key].s));
        $("#slider-segment-end").val(format_time(heater.schedule[day][key].e));

        $("#slider-segment-block").show();
        $("#slider-segment-block-movepos").hide();
    }
});

function slider_update(e) {
    $("#slider-segment-block-movepos").show();
    $("#slider-segment-block").hide();

    if (key !== undefined) {
        var x = e.pageX - $(".slider[day=" + days[day] + "]")[0].offsetLeft;

        var prc = x / slider_width;
        var hour = prc * 24.0;
        hour = Math.round(hour / 0.5) * 0.5;

        if (hour > heater.schedule[day][key - 1].s && hour < heater.schedule[day][key].e) {
            heater.schedule[day][key - 1].e = hour;
            heater.schedule[day][key].s = hour;
            update_slider_ui(day, key);
            changed = 1;
        }
        $("#slider-segment-time").val(format_time(heater.schedule[day][key].s));
    }
}

$("body").on("click", "#slider-segment-ok", function () {

    heater.schedule[day][key].t = $("#slider-segment-temperature").val();
    heater.schedule[day][key].p = $("#slider-segment-power").val();
    var color = color_map(heater.schedule[day][key].t);
    $(".slider[day=" + days[day] + "]").find(".slider-segment[key=" + key + "]").css("background-color", color);

    var time = decode_time($("#slider-segment-start").val());
    if (time != -1 && key > 0 && key < heater.schedule[day].length) {
        if (time >= (heater.schedule[day][key - 1].s + 0.5) && time <= (heater.schedule[day][key].e - 0.5)) {
            heater.schedule[day][key - 1].e = time;
            heater.schedule[day][key].s = time;
        }
    }
    $("#slider-segment-start").val(format_time(heater.schedule[day][key].s));
    update_slider_ui(day, key);

    time = decode_time($("#slider-segment-end").val());
    if (time != -1 && key > 0 && key < (heater.schedule[day].length - 1)) {
        if (time >= (heater.schedule[day][key].s + 0.5) && time <= (heater.schedule[day][key + 1].e - 0.5)) {
            heater.schedule[day][key].e = time;
            heater.schedule[day][key + 1].s = time;
        }
    }
    $("#slider-segment-end").val(format_time(heater.schedule[day][key].e));
    update_slider_ui(day, key + 1);

    save();
    updateclock();

});

$("#slider-segment-movepos-ok").click(function () {

    var time = decode_time($("#slider-segment-time").val());
    if (time != -1 && key > 0) {
        if (time >= (heater.schedule[day][key - 1].s + 0.5) && time <= (heater.schedule[day][key].e - 0.5)) {
            heater.schedule[day][key - 1].e = time;
            heater.schedule[day][key].s = time;
        }
    }
    $("#slider-segment-time").val(format_time(heater.schedule[day][key].s));
    update_slider_ui(day, key);
    save();
});

$("#mode-split").click(function () {
    editmode = 'split';
    $(".editmode").css("background-color", "#555");
    $(this).css("background-color", "#ff9600");
});

$("#mode-move").click(function () {
    editmode = 'move';
    $(".editmode").css("background-color", "#555");
    $(this).css("background-color", "#ff9600");
});

$("#mode-merge").click(function () {
    editmode = 'merge';
    $(".editmode").css("background-color", "#555");
    $(this).css("background-color", "#ff9600");
});

$("#manual_heater").click(function () {
    $(".heatermode").css("background-color", "#555");
    $(this).css("background-color", "#ff9600");
    heater.isOnSchedule = 2;
	
	$(".zone-setpoint").html(heater.target + unit);

    save();
    updateclock();
});

$("#scheduled_heater").click(function () {
    $(".heatermode").css("background-color", "#555");
    $(this).css("background-color", "#ff9600");
    heater.isOnSchedule = 1;
    save();
    updateclock();
});

function color_map(temperature) {
    var f = temperature / 80;
    var a = (1 - f);
    var Y = Math.floor(255 * a);
    r = 255;
    g = Y;
    b = 0;

    return "rgb(" + r + "," + g + "," + b + ")";
}

function update_slider_ui(day, key) {
    if (heater.schedule[day] !== undefined && key < heater.schedule[day].length) {
        var slider = $(".slider[day=" + days[day] + "]");
        if (key > 0) {
            var width = ((heater.schedule[day][key - 1].e - heater.schedule[day][key - 1].s) / 24.0) * 100;
            slider.find(".slider-segment[key=" + (key - 1) + "]").css("width", width + "%");
        }

        var left = (heater.schedule[day][key].s / 24.0) * 100;
        var width = ((heater.schedule[day][key].e - heater.schedule[day][key].s) / 24.0) * 100;
        slider.find(".slider-segment[key=" + key + "]").css("width", width + "%");
        slider.find(".slider-segment[key=" + key + "]").css("left", left + "%");
        slider.find(".slider-button[key=" + key + "]").css("left", left + "%");
    }
}

function format_time(time) {
    var hour = Math.floor(time);
    var mins = Math.round((time - hour) * 60);
    if (mins < 10) mins = "0" + mins;
    return hour + ":" + mins;
}

function decode_time(timestring) {
    var time = -1;
    if (timestring.indexOf(":") != -1) {
        var parts = timestring.split(":");
        var hour = parseInt(parts[0],10);
        var mins = parseInt(parts[1],10);

        if (mins >= 0 && mins < 60 && hour >= 0 && hour < 25) {
            if (hour == 24 && mins !== 0) {} else {
                time = hour + (mins / 60);
            }
        }
    }
    return time;
}

// function for checking if the page is visible or not
// (if not visible it will stop updating data)
function checkVisibility() {
    $(window).bind("focus", function(event) {
        visibleFlag = 1;
    });

    $(window).bind("blur", function(event) {
        visibleFlag = 0;
    });
}

function save() {
	doingsave=true;
    $.ajax({
        type: 'POST',
        url: "/postSettings",
        data: JSON.stringify(heater),
        contentType: "application/json; charset=utf-8",
        dataType: "json",
		cache: false,
        async: true,
        timeout: 3000,
        tryCount : 0,
        retryLimit : 3,			
        success: function (data) {
			doingsave=false;
		},
		error : function(xhr, textStatus, errorThrown ) {
            if (textStatus == 'timeout') {
                this.tryCount++;
                if (this.tryCount <= this.retryLimit) {
                    //try again
                    $.ajax(this);
                    return;
                }            
                return;
            }
            doingsave=false;
		}
    });
}

function server_get() {
	if (visibleFlag) {
		$.ajax({
			url: "/getCurentState",
			dataType: 'json',
			async: true,
			timeout: 3000,
			tryCount : 0,
			retryLimit : 3,				
            success: function (data) {
                if(!doingsave) {
                    if(!data.schedule) data.schedule =  heater.schedule;
                    heater=data;
                    update();
                    updateclock();
                }
			},
            error : function(xhr, textStatus, errorThrown ) {
                if (textStatus == 'timeout') {
                    this.tryCount++;
                    if (this.tryCount <= this.retryLimit) {
                        //try again
                        $.ajax(this);
                        return;
                    }            
                    return;
                }
              
            }
        });
	}
}
