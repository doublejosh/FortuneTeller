(function (doc) {
  const categories = [{
      id: 'heart',
      label: 'Matters of the Heart'
    }, {
      id: 'fortune',
      label: 'Fortune and Fame'
    }, {
      id: 'health',
      label: 'Health and Wellbeing'
    }, {
      id: 'wave',
      label: 'Fate Crashes Over You'
    }, {
      id: 'self',
      label: 'Fate is Made by You'
    }];

  function getScore(votes, shown) {
    if (shown == 0 || votes > shown) return 0;
    return Math.round(parseInt(votes) / parseInt(shown) * 100);
  }

  function showResults(database) {
    database().ref('/fortunes').on('value', function (data) {
      const fortunes = data.val(),
            index = Object.keys(fortunes);
            wrapper = doc.querySelector('.fortunes');

      fortunes.forEach((fortune, i )=> {
        fortunes[i].score = getScore(fortune.votes, fortune.shown);
      });
      fortunes.sort((a, b) => b.score - a.score);

      categories.map((category, i) => {
        let catWrap = doc.createElement("div"),
            header = doc.createElement("h3"),
            total = 0;

        catWrap.classList.add("category");
        catWrap.classList.add(category.id);
        header.innerHTML =  category.label;
        catWrap.appendChild(header);

        let list = doc.createElement("ul");
        fortunes.filter(fortune => fortune.category === category.id).forEach(f => {
          let fortuneCard = doc.createElement("li"),
              scoreBadge = doc.createElement("div"),
              summary = doc.createElement("p");

          total += f.shown;

          summary.innerHTML = f.text + ' (' + f.shown + ')';
          fortuneCard.appendChild(summary);

          if (f.score > 10) {
            scoreBadge.style.width = f.score * 2 + 'px';
          } else {
            scoreBadge.style.width = '20px';
            scoreBadge.classList.add("low-score");
          }
          scoreBadge.classList.add("score-badge");
          scoreBadge.innerHTML = f.score + '%';

          fortuneCard.appendChild(scoreBadge);
          list.appendChild(fortuneCard);
        });
        catWrap.appendChild(list);

        let count = doc.createElement("div");
        count.classList.add("count-badge");
        count.innerHTML = 'Shown: ' + total;
        catWrap.appendChild(count);

        wrapper.appendChild(catWrap);
      });
    });
  }

  doc.addEventListener('DOMContentLoaded', function() {
    try {
      doc.getElementById('load').innerHTML = 'Data Loaded';
      showResults(firebase.database);
    } catch (e) {
      console.error(e);
      doc.getElementById('load').innerHTML =
          'Error loading the Firebase SDK, check the console.';
    }
  });
})(document);