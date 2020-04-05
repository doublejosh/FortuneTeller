import { combineReducers } from 'redux'
import { firebaseReducer } from 'react-redux-firebase'
import theme from './theme'

const rootReducer = combineReducers({
	firebase: firebaseReducer,
	theme,
})

export default rootReducer
