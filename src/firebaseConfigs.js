import firebase from 'firebase/app';
import 'firebase/database';

const firebaseConfig = {
    apiKey: 'AIzaSyABlz0TjYVXox7P_arw-ReeKQ8e8hHWJps',
    authDomain: 'fortunecalibrator.firebaseapp.com',
    databaseURL: 'https://fortunecalibrator.firebaseio.com',
    projectId: 'fortunecalibrator',
    storageBucket: 'fortunecalibrator.appspot.com',
    messagingSenderId: '65442706036',
    appId: '1:65442706036:web:1481d3208ba81f9b191721',
    measurementId: 'G-WVXSGZ81EQ',
};

firebase.initializeApp(firebaseConfig);

export default firebase;
