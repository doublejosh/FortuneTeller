import React from 'react'
import { BrowserRouter as Router, Switch, Route } from 'react-router-dom'
import Logo from './Logo'
import Categories from './Dashboard/Categories'
import Links from './Links'
import Vote from './Survey/Vote'
import Add from './Survey/Add'
import Results from './Survey/Results'
import Test from './Test'
import '../style.scss'

export default () => (
	<Router>
		<Switch>
			<Route path="/vote">
				<Vote />
			</Route>
			<Route path="/results">
				<Results />
			</Route>
			<Route path="/add">
				<Add />
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
