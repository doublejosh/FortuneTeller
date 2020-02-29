import React from 'react';
import ReactDOM from 'react-dom';
import App from './components/App';

// Data via https://react-redux-firebase.com
import { Provider } from 'react-redux';
import { createStore } from 'redux';
import rootReducer from './store/rootReducer';
import { ReactReduxFirebaseProvider } from 'react-redux-firebase';
import firebase from './firebaseConfigs';

const store = createStore(rootReducer, {});
const reactReduxFirebaseProps = {
    firebase,
    config: {
        enableLogging: false,
    },
    dispatch: store.dispatch,
};

ReactDOM.render(
    <Provider store={store}>
        <ReactReduxFirebaseProvider {...reactReduxFirebaseProps}>
            <App />
        </ReactReduxFirebaseProvider>
    </Provider>,
    document.getElementById('root')
);
