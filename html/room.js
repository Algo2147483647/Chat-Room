$(document).ready(function(){
    var t = 0;

    setInterval(function(){
        $.ajax({
            url : "http://192.168.222.128:9999/room.html",
            type : "get",
            data : {
                action  : "get_message",
                m_id: t
            },
            success:function(result) {
                if (result != "") {
                    resultList = result.split("&");

                    for (var i = 0; i < resultList.length; i++)
                        if (resultList[i] != "") {
                            str = decodeURIComponent(resultList[i].replace(/\+/g, ' '));
                            $("#chat_list").append("<li>" + str + "</li>");
                        }
                    t = t + resultList.length - 1;
                }
            }
        });
    },1000);
});

function send() {
    var $message = $("#textarea");

    if ($message.val() == "") {
        return;
    }
    else {
        $.ajax({
            url : "http://192.168.222.128:9999/room.html",
            type : "post",
            data : {
                action  : "message",
                message: $message.val()
            },
            success:function(result) {
                alert(result);
            }
        })
    }
}