import React from 'react'
import { BrowserRouter as Router, Switch, Route } from 'react-router-dom'
import Logo from './Logo'
import Categories from './Categories'
import Links from './Links'
import Survey from './Survey'
import '../style.scss'

export default () => {
	return (
		<Router>
			<Switch>
				<Route path="/vote">
					<Survey />
				</Route>
				<Route path="/">
					<Logo />
					<Categories />
					<Links />
				</Route>
			</Switch>
		</Router>
	)
}
