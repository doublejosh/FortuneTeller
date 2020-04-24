import React, { useState, useEffect, useRef } from 'react'
import { useSelector } from 'react-redux'
import { useLocation } from 'react-router-dom'
import { useFirebaseConnect } from 'react-redux-firebase'
import firebase from '../../firebaseConfigs'
import { css } from 'glamor'
import CategoryName from './CategoryName'

export default () => {
	const theme = useSelector(state => state.theme) || []
	useFirebaseConnect(['categories'])
	const categories = useSelector(state => state.firebase.data.categories)
	const [fortune, setFortune] = useState()
	const [category, setCategory] = useState()

	const { pathname } = useLocation()
	const inputRef = useRef(null)

	//jhgjhg
	useEffect(() => {
		if (!category) inputRef.current.focus()
	}, [pathname])

	return (
		<div
			{...css({
				display: 'grid',
				gridTemplateAreas: `"input input"
														"categories categories"
														"button none"`,
				gridTemplateColumns: '1fr auto',
				gridRowGap: '1rem',
				padding: '1.5rem',
				paddingBottom: '10rem',
			})}>
			<textarea
				id="newFortune"
				ref={inputRef}
				onChange={e => setFortune(e.target.value)}
				rows="6"
				{...css({
					gridArea: 'input',
					padding: '2rem',
					[`@media(min-width: ${theme.breaks.md}px)`]: {
						padding: '4rem',
					},
					fontSize: '2rem',
					border: 'none',
					borderRadius: theme.borderRadius,
					outline: 0,
				})}>
				{fortune}
			</textarea>

			{categories ? (
				<section {...css({ gridArea: 'categories' })}>
					<h3
						{...css({
							textTransform: 'uppercase',
							margin: '0 0 0 3rem',
							[`@media(min-width: ${theme.breaks.md}px)`]: {
								marginLeft: '5.25rem',
							},
						})}>
						Category
					</h3>
					{categories.map(c => {
						const props = { ...c, setCategory, isActive: category === c.id }
						return <CategoryName key={c.id} {...props} />
					})}
				</section>
			) : null}

			<button
				onClick={() => {
					if (fortune) {
						firebase
							.database()
							.ref('fortunes')
							.push({
								text: fortune,
								shown: 0,
								votes: 0,
								category: category,
							})
					}
					setFortune('')
					setCategory('')
					document.getElementById('newFortune').value = ''
				}}
				{...css({
					gridArea: 'button',
					fontSize: '2rem',
					border: 'none',
					padding: '1rem 4rem',
					borderRadius: theme.borderRadius,
					background: '#000',
					color: '#FFF',
					outline: 'none',
					'&:hover': { background: '#111', color: '#DDD' },
					'&:active': { filter: 'invert(100%)' },
				})}>
				Add Fortune
			</button>
		</div>
	)
}
