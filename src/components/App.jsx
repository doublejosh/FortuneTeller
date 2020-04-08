import React from 'react'
import { BrowserRouter as Router, Switch, Route } from 'react-router-dom'
import Logo from './Logo'
import Categories from './Categories'
import Links from './Links'
import Survey from './Survey'
import Test from './Test'
import '../style.scss'

export default () => (
	<Router>
		<Switch>
			<Route path="/vote">
				<Survey />
			</Route>
			<Route path="/test">
				<Test />
			</Route>
			<Route path="/">
				<Logo />
				<Categories />
				<Links />
			</Route>
		</Switch>
	</Router>
)
