import React from 'react'
import Logo from './Logo'
import Categories from './Categories'
import Links from './Links'
import '../style.scss'

export default () => {
	return (
		<React.Fragment>
			<Logo />
			<Categories />
			<Links />
		</React.Fragment>
	)
}
