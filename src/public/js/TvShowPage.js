function TvShowPage() {
    this.tvShow = null;
}

TvShowPage.prototype.bindEvents = function() {
    this.keyDownListener = function(event) {
        ListUtils.handleKeyDown(event);
    }
    window.addEventListener("keydown", this.keyDownListener);
}

TvShowPage.prototype.unbindEvents = function() {
    window.removeEventListener("keydown", this.keyDownListener);
}

TvShowPage.prototype.createNodes = function() {
    var self = this;
    var page = $(".page");
    
    var backButton = $("<input class='backButton' type='button'/>");
    backButton.attr("value", "back");
    backButton.click(function() {
        window.location.hash = "#!/";
    });
    
    var playButton = $("<input class='playButton' type='button'/>");
    playButton.attr("value", "play");
    playButton.attr("disabled", "disabled");
    
    playButton.click(function() {
        var first = $("li .toggleWatchedButton:not(.watched):first").parent();
        var files = [];
        files.push(first.attr("data-filename"));
        first.nextAll("li").each(function() {
            files.push(this.getAttribute("data-filename"));
        });
        self.play(files);
    });
    
    var completeButton = document.createElement("input");
    completeButton.type = "button";
    completeButton.value = "set completed";
    completeButton.setAttribute("disabled", "disabled");
    
    completeButton.onclick = function() {
        $("li .toggleWatchedButton:not(.watched)").each(function() {
            this.click();
        });
    }
    
    var subtitleTrackField = $("<input type=\"number\"/>");
    var audioTrackField = $("<input type=\"number\"/>");
    $.getJSON("api/page/playerSettings", function(data) {
        subtitleTrackField.get(0).value = data.subtitleTrack;
        audioTrackField.get(0).value = data.audioTrack;
        
        function setFn(key) {
            return function() {
                var number = this.value;
                data[key] = number;
                var json = JSON.stringify(data);
                $.ajax({
                    url:"api/page/playerSettings",
                    type: "PUT",
                    data: json
                }).complete(function() {
                    console.log("set subtitle to ", number);
                });
            }
        }
        
        subtitleTrackField.on("input", setFn("subtitleTrack"));
        audioTrackField.on("input", setFn("audioTrack"));
    });
    
    var seasonsEl = $(document.createElement("div"));
    seasonsEl.addClass("seasons");
    
    $.getJSON("api/page/showDetails", function(data) {
        console.log(data);
        self.tvShow = data;
        playButton.removeAttr("disabled");
        completeButton.removeAttribute("disabled");
        self.createSeasonList(data.episodes, seasonsEl);
    });

    page.append(backButton);
    page.append(playButton);
    page.append("<span> sub: </span>");
    page.append(subtitleTrackField);
    page.append("<span> audio: </span>");
    page.append(audioTrackField);
    page.append(seasonsEl);
    page.append(completeButton);
    this.bindEvents();
}

TvShowPage.prototype.removeNodes = function() {
    this.unbindEvents();
}

TvShowPage.prototype.createSeasonList = function(episodes, seasonsEl) {
    var self = this;
    var seasonEl = $(document.createElement("ul"));
    seasonEl.addClass("season");
    
    episodes = episodes.sort(function(a,b) {
        return a.episodeNumber < b.episodeNumber ? -1 : 1;
    })
    
    $.each(episodes, function() {
        var ep = this;
        var episodeEl = $(document.createElement("li"));
        var text = $(document.createElement("span"));
        
        var title = 
            ep.episodeNumber + " " +
            ep.showName + " " +
            (ep.episodeName ? ("- " + ep.episodeName + " ") : "") +
            ep.releaseGroup;
        text.text(title);
        
        
        function watchButtonSetDisplay() {
            if (ep.watched) {
                toggleWatchedButton.text("+ ");
                toggleWatchedButton.addClass("watched");
            } else {
                toggleWatchedButton.text("- ");
                toggleWatchedButton.removeClass("watched");
            }
        }
        
        var toggleWatchedButton = $(document.createElement("span"));
        toggleWatchedButton.get(0).className = "textButton toggleWatchedButton";
        watchButtonSetDisplay();
        toggleWatchedButton.click(function(event) {
            ep.watched = !ep.watched;
            watchButtonSetDisplay();
            $.getJSON("api/library/toggleWatched?" + ep.path);
        });
        
        episodeEl.append(toggleWatchedButton);
        episodeEl.append(text);
        
        episodeEl.attr("data-fileName", this.path);
        
        episodeEl.click(function(event) {
            if (!event.target.classList.contains("textButton")) {
                self.play($(this).nextAll("li").andSelf().map(function() {
                    return this.getAttribute("data-fileName");
                }).toArray());
            }
        });
        
        episodeEl.attr("tabindex", "1");
        episodeEl.on("focus", function() {
            $(this).mousemove();
        });
        episodeEl.mousemove(function() {
            $("li.focused").removeClass("focused");
            $(this).addClass("focused");
        });
        
        seasonEl.append(episodeEl);
    });
    seasonsEl.append(seasonEl);
}

TvShowPage.prototype.play = function(episode) {
    if (G.playerType == "stream") {
        alert("gu")
        G.video = "/video/" + episode; // TODO DON'T SAVE IT HERE
        window.location.hash = "#!/StreamPlayerPage/";
    } else {
        if (episode instanceof Array) {
            var json = {
                tvShow: this.tvShow.name,
                episodes: episode
            }
            
            $.getJSON("api/player/setPlaylist?" + JSON.stringify(json), function(data) {
                if (!data.error) {
                    window.location.hash = "#!/PlayerPage";
                }
            });
        
            /*
            $.ajax({
                url: "api/player/setPlaylist",
                type:"POST",
                data:JSON.stringify(json),
                processData: false
            }).done(function(data) {
                console.log("set playlist got response:", data)
                if (!data.error) {
                    window.location.hash = "#!/PlayerPage";
                }
            });
            */
            
        } else {
            var json = {
                tvShow: this.tvShow.name,
                filename: episode
            }
        
            $.getJSON("api/player/play?" + JSON.stringify(json), function(data) {
                if (!data.error) {
                    window.location.hash = "#!/PlayerPage";
                }
            });
        }
    }
}
